#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void *thread_function(void *arg) {
    int id = *(int *)arg;
    pthread_mutex_t *first_mutex, *second_mutex;

    if (id == 1) {
        first_mutex = &mutex1;
        second_mutex = &mutex2;
    } else {
        first_mutex = &mutex2;
        second_mutex = &mutex1;
    }

    pthread_mutex_lock(first_mutex);
    printf("Thread %d: Locked first_mutex\n", id);

    sleep(1); // Имитация работы

    printf("Thread %d: Trying to lock second_mutex\n", id);
    pthread_mutex_lock(second_mutex);
    printf("Thread %d: Locked second_mutex\n", id);

    // Критическая секция
    printf("Thread %d: In critical section\n", id);

    pthread_mutex_unlock(second_mutex);
    pthread_mutex_unlock(first_mutex);

    return NULL;
}

int main() {
    pthread_t thread1, thread2;
    int id1 = 1, id2 = 2;

    pthread_create(&thread1, NULL, thread_function, &id1);
    pthread_create(&thread2, NULL, thread_function, &id2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Main: Both threads finished\n");

    return 0;
}