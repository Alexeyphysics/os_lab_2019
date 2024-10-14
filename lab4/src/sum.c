#include "sum.h"

int Sum(const int *arr, int begin, int end) {
  int sum = 0;
  for (int i = begin; i < end; i++) {
    sum += arr[i];
  }
  return sum;
}