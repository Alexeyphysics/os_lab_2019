#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>
#include <fcntl.h>

#include "find_min_max.h"
#include "utils.h"

// Структура для хранения минимального и максимального значений
typedef struct {
  int min;
  int max;
} MinMaxResult;

// Функция для поиска минимального и максимального значений в части массива
MinMaxResult find_min_max_part(const int *array, int start, int end) {
  MinMaxResult result = {INT_MAX, INT_MIN};
  for (int i = start; i < end; i++) {
    if (array[i] < result.min) result.min = array[i];
    if (array[i] > result.max) result.max = array[i];
  }
  return result;
}

// Функция для записи данных в канал
ssize_t write_to_pipe(int fd, const void *buf, size_t count) {
  ssize_t bytes_written = 0;
  while (bytes_written < count) {
    ssize_t result = write(fd, (char *)buf + bytes_written,
                          count - bytes_written);
    if (result == -1) {
      if (errno == EINTR) {
        continue;  // Повтор попытки, если прервано сигналом
      } else {
        perror("write");
        return -1;
      }
    } else if (result == 0) {
      return bytes_written;
    }
    bytes_written += result;
  }
  return bytes_written;
}

// Функция для чтения данных из канала
ssize_t read_from_pipe(int fd, void *buf, size_t count) {
  ssize_t bytes_read = 0;
  while (bytes_read < count) {
    ssize_t result = read(fd, (char *)buf + bytes_read, count - bytes_read);
    if (result == -1) {
      if (errno == EINTR) {
        continue;  // Повтор попытки, если прервано сигналом
      } else {
        perror("read");
        return -1;
      }
    } else if (result == 0) {
      return bytes_read;
    }
    bytes_read += result;
  }
  return bytes_read;
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  // Обработка аргументов командной строки
  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {
        {"seed", required_argument, 0, 0},
        {"array_size", required_argument, 0, 0},
        {"pnum", required_argument, 0, 0},
        {"by_files", no_argument, 0, 'f'},
        {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            if (seed <= 0) {
              printf("Ошибка: Некорректное значение seed\n");
              return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            if (array_size <= 0) {
              printf("Ошибка: Некорректное значение array_size\n");
              return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            if (pnum <= 0) {
              printf("Ошибка: Некорректное значение pnum\n");
              return 1;
            }
            break;
          case 3:
            with_files = true;
            break;

          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }


  int *array = malloc(sizeof(int) * array_size);

  GenerateArray(array, array_size, seed);

  int active_child_processes = 0;
  struct timeval start_time;
  gettimeofday(&start_time, NULL);


  int pipefd[pnum][2];
  char *filenames[pnum];

  for (int i = 0; i < pnum; i++) {
    filenames[i] = NULL;
    if (with_files) {

      asprintf(&filenames[i], "tempfile_%d.txt", i);
      if (filenames[i] == NULL) {
        perror("asprintf");
        exit(EXIT_FAILURE);
      }
    } else {

      if (pipe(pipefd[i]) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
      }
    }

    pid_t child_pid = fork();

    if (child_pid >= 0) {
      active_child_processes += 1;

      if (child_pid == 0) {

        int part_size = array_size / pnum;
        int start = i * part_size;
        int end = (i == pnum - 1) ? array_size : start + part_size;

        MinMaxResult result = find_min_max_part(array, start, end);

        if (with_files) {

          FILE *fp = fopen(filenames[i], "wb");
          if (fp == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
          }
          if (fwrite(&result, sizeof(result), 1, fp) != 1) {
            perror("fwrite");
            exit(EXIT_FAILURE);
          }
          fclose(fp);
        } else {

          close(pipefd[i][0]);
          if (write_to_pipe(pipefd[i][1], &result, sizeof(result)) == -1) {
            exit(EXIT_FAILURE);
          }
          close(pipefd[i][1]);
        }

        return 0; 

      } else {

        if (!with_files) {
          close(pipefd[i][1]);
        }
      }
    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }


  while (active_child_processes > 0) {
    wait(NULL);
    active_child_processes -= 1;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;


  for (int i = 0; i < pnum; i++) {
    MinMaxResult result;

    if (with_files) {

      FILE *fp = fopen(filenames[i], "rb");
      if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
      }
      if (fread(&result, sizeof(result), 1, fp) != 1) {
        perror("fread");
        exit(EXIT_FAILURE);
      }
      fclose(fp);

      remove(filenames[i]);
      free(filenames[i]);

    } else {

      if (read_from_pipe(pipefd[i][0], &result, sizeof(result)) == -1) {
        exit(EXIT_FAILURE);
      }
      close(pipefd[i][0]);
    }


    if (result.min < min_max.min) min_max.min = result.min;
    if (result.max > min_max.max) min_max.max = result.max;
  }


  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);
  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;
  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}