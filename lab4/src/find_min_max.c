#include "find_min_max.h"

#include <limits.h>

struct MinMax GetMinMax(int *array, unsigned int begin, unsigned int end) {
  // Структура для хранения минимального и максимального значений
  struct MinMax min_max;
  // Инициализируем минимальное значение максимальным возможным значением int
  min_max.min = INT_MAX;
  // Инициализируем максимальное значение минимальным возможным значением int
  min_max.max = INT_MIN;

  // Проходим по всем элементам массива от begin до end
  for (unsigned int i = begin; i < end; i++) {
    // Если текущий элемент меньше минимального, обновляем минимальное
    if (array[i] < min_max.min) {
      min_max.min = array[i];
    }
    // Если текущий элемент больше максимального, обновляем максимальное
    if (array[i] > min_max.max) {
      min_max.max = array[i];
    }
  }

  // Возвращаем структуру с минимальным и максимальным значениями
  return min_max;
}