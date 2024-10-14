#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>

#include "find_min_max.h"

// Структура для хранения аргументов командной строки
struct CmdArgs {
  int timeout; // Таймаут в секундах
  int num_processes; // Количество дочерних процессов 
  pid_t *pids; // Массив PID дочерних процессов
};

// Глобальная переменная для доступа к аргументам командной строки в обработчике сигнала
struct CmdArgs args;

// Обработчик сигнала SIGALRM
void alarm_handler(int signum) {
  fprintf(stderr, "Время ожидания истекло! (%d секунд)\n", args.timeout);
  // Отправка SIGKILL всем дочерним процессам
  for (int i = 0; i < args.num_processes; i++) {
    kill(args.pids[i], SIGKILL);
  }
  exit(EXIT_FAILURE);
}

// Функция разбора аргументов командной строки
struct CmdArgs parse_args(int argc, char *argv[]) {
  struct CmdArgs args = {0}; // Инициализируем структуру нулями
  int opt;

  // Структура для getopt_long
  static struct option long_options[] = {
    {"timeout", required_argument, 0, 't'},
    {0, 0, 0, 0}
  };

  // Разбираем аргументы командной строки
  while ((opt = getopt_long(argc, argv, "t:", long_options, NULL)) != -1) {
    switch (opt) {
      case 't':
        args.timeout = atoi(optarg);
        break;
      default:
        fprintf(stderr, "Usage: %s [--timeout <seconds>]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  return args;
}

int main(int argc, char *argv[]) {
  // Разбираем аргументы командной строки
  args = parse_args(argc, argv);

  // Количество дочерних процессов
  int num_processes = 4; 
  args.num_processes = num_processes;

  // Массив для хранения идентификаторов процессов (PID)
  pid_t pids[num_processes]; 
  args.pids = pids; 

  // Массив для хранения границ подмассивов
  unsigned int segments[num_processes + 1]; 

  // Создание массива для поиска минимума и максимума
  int *array = malloc(40000000 * sizeof(int)); 
  if (array == NULL) {
    perror("Ошибка выделения памяти");
    return 1;
  }

  // Заполнение массива случайными числами
  for (int i = 0; i < 40000000; i++) {
    array[i] = rand();
  }

    // Заполнение массива segments
  for (int i = 0; i <= num_processes; ++i) {
    segments[i] = i * (40000000 / num_processes); 
  }

  // Создание дочерних процессов
  for (int i = 0; i < num_processes; i++) {
    pids[i] = fork();

    if (pids[i] == 0) {
      // Код дочернего процесса
      struct MinMax local_min_max = GetMinMax(array, segments[i], segments[i + 1]);
      printf("Дочерний процесс %d (PID %d): min = %d, max = %d\n", i + 1, getpid(), local_min_max.min, local_min_max.max);
      exit(0); 
    } else if (pids[i] < 0) {
      perror("Ошибка при создании процесса");
      return 1; 
    }
  }

  // Родительский процесс

  // Установка таймера, если таймаут указан
  if (args.timeout > 0) {
    alarm(args.timeout);
    signal(SIGALRM, alarm_handler);
  }

  // Ожидание завершения дочерних процессов
  for (int i = 0; i < num_processes; i++) {
    wait(NULL); 
  }

  // Вывод сообщения об успешном завершении
  printf("Все дочерние процессы завершили работу.\n");

  free(array); 
  return 0;
}