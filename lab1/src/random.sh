#!/bin/bash

# Создаем файл numbers.txt
> numbers.txt

# Генерируем 150 случайных чисел
for i in $(seq 1 150); do
  # Генерируем случайное число
  random_number=$(head -c 4 /dev/urandom | od -An -tu1)
  # Добавляем число в файл
  echo "$random_number" >> numbers.txt
done

echo "150 случайных чисел записаны в файл numbers.txt"