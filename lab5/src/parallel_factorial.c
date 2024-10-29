#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>

typedef struct {
    int start;
    int end;
    long long *result;
    pthread_mutex_t *mutex;
} ThreadArgs;

void *compute_factorial(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    long long local_result = 1;

    for (int i = args->start; i <= args->end; i++) {
        local_result *= i;
    }

    pthread_mutex_lock(args->mutex);
    *args->result *= local_result;
    pthread_mutex_unlock(args->mutex);

    return NULL;
}

int main(int argc, char *argv[]) {
    int k = 0, pnum = 0, mod = 0;
    int opt;

    static struct option long_options[] = {
        {"k", required_argument, 0, 'k'},
        {"pnum", required_argument, 0, 'p'},
        {"mod", required_argument, 0, 'm'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "k:p:m:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'k':
                k = atoi(optarg);
                break;
            case 'p':
                pnum = atoi(optarg);
                break;
            case 'm':
                mod = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -k <number> --pnum=<threads> --mod=<mod>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (k <= 0 || pnum <= 0 || mod <= 0) {
        fprintf(stderr, "Usage: %s -k <number> --pnum=<threads> --mod=<mod>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pthread_t threads[pnum];
    ThreadArgs args[pnum];
    long long result = 1;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    int chunk_size = k / pnum;
    int remainder = k % pnum;

    for (int i = 0; i < pnum; i++) {
        args[i].start = i * chunk_size + 1;
        args[i].end = (i + 1) * chunk_size;
        if (i == pnum - 1) {
            args[i].end += remainder;
        }
        args[i].result = &result;
        args[i].mutex = &mutex;

        if (pthread_create(&threads[i], NULL, compute_factorial, &args[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < pnum; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }

    result %= mod;

    printf("Factorial of %d modulo %d is %lld\n", k, mod, result);

    return 0;
}