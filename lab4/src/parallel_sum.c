#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <getopt.h>

#include "sum.h" // Подключаем библиотеку для подсчета суммы

// Структура для хранения аргументов потока
struct SumArgs {
  int *array;
  int begin;
  int end;
};

// Функция для генерации массива
void GenerateArray(int *array, unsigned int array_size, unsigned int seed) {
  srand(seed);
  for (int i = 0; i < array_size; i++) {
    array[i] = rand();
  }
}

// Функция потока для подсчета частичной суммы
void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  // Вызываем функцию Sum из библиотеки sum.h
  return (void *)(size_t)Sum(sum_args->array, sum_args->begin, sum_args->end); 
}

int main(int argc, char **argv) {
  uint32_t threads_num = 0;
  uint32_t array_size = 0;
  uint32_t seed = 0;

  // Разбор аргументов командной строки
  int opt;
  while ((opt = getopt(argc, argv, "t:s:a:")) != -1) {
    switch (opt) {
      case 't':
        threads_num = atoi(optarg);
        break;
      case 's':
        seed = atoi(optarg);
        break;
      case 'a':
        array_size = atoi(optarg);
        break;
      default:
        fprintf(stderr, "Использование: %s -t <threads_num> -s <seed> -a <array_size>\n", argv[0]);
        return 1;
    }
  }

  // Проверка на корректность аргументов
  if (threads_num <= 0 || array_size <= 0) {
    fprintf(stderr, "Ошибка: Некорректные аргументы.\n");
    return 1;
  }

  pthread_t threads[threads_num];
  int *array = malloc(sizeof(int) * array_size);

  // Генерация массива
  GenerateArray(array, array_size, seed); 

  // Подготовка аргументов для потоков
  struct SumArgs args[threads_num];
  int chunk_size = array_size / threads_num;
  for (uint32_t i = 0; i < threads_num; i++) {
    args[i].array = array;
    args[i].begin = i * chunk_size;
    args[i].end = (i == threads_num - 1) ? array_size : (i + 1) * chunk_size;
  }

  // Замер времени начала подсчета суммы
  struct timespec start_time, end_time;
  clock_gettime(CLOCK_MONOTONIC, &start_time);

  // Создание потоков
  for (uint32_t i = 0; i < threads_num; i++) {
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
      perror("Ошибка при создании потока");
      free(array);
      return 1;
    }
  }

  // Ожидание завершения потоков и суммирование результатов
  int total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    int sum = 0;
    pthread_join(threads[i], (void **)&sum);
    total_sum += sum;
  }

  // Замер времени окончания подсчета суммы
  clock_gettime(CLOCK_MONOTONIC, &end_time);
  double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + 
                        (end_time.tv_nsec - start_time.tv_nsec) / 1e9; 

  free(array);
  printf("Total: %d\n", total_sum);
  printf("Время подсчета: %f секунд\n", elapsed_time);
  return 0;
}