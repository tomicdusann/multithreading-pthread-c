#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define THREAD_NUM 4

int mails = 0;
pthread_mutex_t mutex;

void* procedure() {
    for (int i = 0; i < 10000000; i++) {
        pthread_mutex_lock(&mutex);
        mails++;
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char* argv[]) {
    pthread_t th[THREAD_NUM];
    int i;
    pthread_mutex_init(&mutex, NULL);
    
    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_create(th + i, NULL, &procedure, NULL) != 0) {
            perror("Failed to create thread");
            return 1;
        }
        printf("Thread %d has started\n", i);
    }
    
    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            return 2;
        }
        printf("Thread %d has finished execution\n", i);
    }
    
    pthread_mutex_destroy(&mutex);
    printf("Number of mails: %d\n", mails);
    return 0;
}

/*
OUTPUT:
pokrene se nit 0
pokrene se nit 1
pokrene se nit 2
pokrene se nit 3
zavrsi se nit 0
zavrsi se nit 1
zavrsi se nit 2
zavrsi se nit 3

Ovo je ispravan nacin jer se niti pokrecu kroz jedu petlju i join-uju kroz drugu, na taj nacin se obezbedjuje konkurentno izvrsavanje niti, jer se nit 
pokrece jedna za drugom bez cekanja da prethodna zavrsi, sto znaci da je izvrsavanje programa mnogo brze i procesor je efikasnije iskoriscen.
*/