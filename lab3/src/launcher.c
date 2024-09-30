#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <program> <seed> <array_size>\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    } else if (pid == 0) {


        char *args[] = {argv[1], argv[2], argv[3], NULL};
        execv(argv[1], args);

        perror("execv");
        exit(1);
    } else {

        wait(NULL);  
        printf("Дочерний процесс завершился.\n");
    }

    return 0;
}