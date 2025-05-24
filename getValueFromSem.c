#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>

#define THREAD_NUM 4

sem_t sem;

void* routine(void* args) {
    int index = *(int*)args;
    int semVal;
    
    sem_wait(&sem);
    sem_getvalue(&sem, &semVal); // funkcija za dohvatanje trenutne vrednosti semafora
    printf("(%d) Current semaphore value(propusnica) is %d\n", index, semVal);
    sem_post(&sem);

    free(args);
}

int main(int argc, char *argv[]) {
    pthread_t th[THREAD_NUM];
    sem_init(&sem, 0, 4);
    
    for (int i = 0; i < THREAD_NUM; i++) {
        int* a = malloc(sizeof(int));
        *a = i;
        if (pthread_create(&th[i], NULL, &routine, a) != 0) {
            perror("Failed to create thread");
        }
    }

    for (int i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
    
    sem_destroy(&sem);
    return 0;
}