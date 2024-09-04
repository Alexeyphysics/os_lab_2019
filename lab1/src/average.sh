#!/bin/bash

# Проверка наличия аргументов
if [ $# -eq 0 ]; then
  echo "Ошибка: Необходимо передать аргументы."
  exit 1
fi

# Инициализация переменных
sum=0
count=0

# Суммирование аргументов и подсчет их количества
for arg in "$@"; do
  sum=$((sum + arg))
  count=$((count + 1))
done

# Вычисление среднего арифметического
if (( count > 0 )); then
  average=$((sum/$#))
  #average=$(echo "scale=2; $sum / $count" | bc)
else
  average=0
fi

# Вывод результатов
echo "Количество аргументов: $count"
echo "Среднее арифметическое: $sum"
echo "Среднее арифметическое: $average"