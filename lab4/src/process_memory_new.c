#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>  

// Макрос для вывода адреса переменной
#define SHW_ADR(ID, I) (printf("ID %s \t is at virtual address: %p\n", ID, (void*)&I))

// Объявление переменных, определяющих границы сегментов памяти (определяются компоновщиком)
extern int etext, edata, end; 

// Инициализированный указатель на строку (статическая память)
char *cptr = "This message is output by the function showit()\n"; 
// Массив символов (статическая память, неинициализированный)
char buffer1[25];
// Прототип функции showit
int showit(char *p);

int main() {
  // Автоматическая переменная на стеке
  int i = 0; 

  // Вывод адресов границ сегментов памяти
  printf("\nAddress etext: %p \n", (void*)&etext); 
  printf("Address edata: %p \n", (void*)&edata);
  printf("Address end  : %p \n", (void*)&end);

  // Вывод адресов функций и переменных
  SHW_ADR("main", main);
  SHW_ADR("showit", showit);
  SHW_ADR("cptr", cptr);
  SHW_ADR("buffer1", buffer1);
  SHW_ADR("i", i);

  // Копирование строки в buffer1
  strcpy(buffer1, "A demonstration\n");  

  // Вывод строки с помощью системного вызова write()
  ssize_t bytes_written = write(1, buffer1, strlen(buffer1) + 1);
  // Проверка на ошибки записи
  if (bytes_written == -1) {
      perror("Ошибка write");
      return 1; // Код ошибки
  }

  // Вызов функции showit
  showit(cptr);

  // Успешное завершение программы
  return 0; 
} 

// Функция для демонстрации динамического выделения памяти
int showit(char *p) {
  // Указатель на динамически выделенную память
  char *buffer2;
  // Вывод адреса указателя buffer2 (еще не выделена память)
  SHW_ADR("buffer2", buffer2);

  // Выделение памяти для строки p + 1 байт для нуль-терминатора
  if ((buffer2 = (char *)malloc((unsigned)(strlen(p) + 1))) != NULL) {
    // Вывод адреса выделенной памяти
    printf("Alocated memory at %p\n", (void*)buffer2);
    // Копирование строки p в выделенную память
    strcpy(buffer2, p); 
    // Вывод строки
    printf("%s", buffer2);
    // Освобождение выделенной памяти
    free(buffer2);
  } else {
    // Обработка ошибки выделения памяти
    printf("Allocation error\n");
    exit(1); // Код ошибки
  }
}