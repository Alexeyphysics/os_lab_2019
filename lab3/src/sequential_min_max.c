#include <stdio.h>
#include <stdlib.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("Usage: %s seed arraysize\n", argv[0]);
    return 1;
  }

  // atoi() может вызвать undefined behavior для нечисловых строк
  // Лучше использовать strtol() для более безопасного преобразования
  char *endptr;
  long seed = strtol(argv[1], &endptr, 10); 
  if (*endptr != '\0' || seed <= 0 || seed > INT_MAX) { 
    printf("Seed должно быть положительным целым числом.\n");
    return 1;
  }

  long array_size = strtol(argv[2], &endptr, 10);
  if (*endptr != '\0' || array_size <= 0 || array_size > INT_MAX) {
    printf("Array_size должно быть положительным целым числом.\n");
    return 1;
  }

  int *array = malloc(array_size * sizeof(int));
  if (array == NULL) {
    printf("Ошибка выделения памяти.\n");
    return 1;
  }

  GenerateArray(array, array_size, (int)seed);
  struct MinMax min_max = GetMinMax(array, 0, array_size);
  free(array);

  printf("min: %d\n", min_max.min);
  printf("max: %d\n", min_max.max);

  return 0;
}