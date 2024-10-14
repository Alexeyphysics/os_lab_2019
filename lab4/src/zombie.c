#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
  pid_t child_pid = fork();

  if (child_pid == 0) {
    // Дочерний процесс
    printf("Дочерний процесс (PID: %d) запущен.\n", getpid());
    sleep(2); // Работаем некоторое время
    printf("Дочерний процесс (PID: %d) завершается.\n", getpid());
    exit(0); 
  } else if (child_pid > 0) {
    // Родительский процесс
    printf("Родительский процесс (PID: %d) запущен. Дочерний процесс: %d\n", getpid(), child_pid);

    
    //int status;
    //waitpid(child_pid, &status, 0); // Ожидаем завершения дочернего процесса
    //printf("Дочерний процесс завершился. Родительский процесс завершается.\n");



    
    // Не вызываем wait() или waitpid()!

    // Бесконечный цикл, чтобы родительский процесс не завершался и могли видеть процесс
    while (1) {
      sleep(1); 
      printf("Родительский процесс работает...\n");
    } 
  } else {
    perror("Ошибка fork");
    return 1;
  }

  return 0;
}