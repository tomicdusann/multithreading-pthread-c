#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define THREAD_NUM 4

int counter = 0;
pthread_mutex_t mutex;

void* procedure() {
    for (int i = 0; i < 10000000; i++) {
        pthread_mutex_lock(&mutex);
        counter++;
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
        
        if (pthread_join(th[i], NULL) != 0) {			
            return 2;
        }
        printf("Thread %d has finished execution\n", i);
    }
    
    
    pthread_mutex_destroy(&mutex);
    printf("Counter value: %d\n", counter);
    return 0;
}

/*
OUTPUT uvek:

Thread 0 has started
Thread 0 has finished execution
Thread 1 has started
Thread 1 has finished execution
Thread 2 has started
Thread 2 has finished execution
Thread 3 has started
Thread 3 has finished execution
Counter value: 40000000


Vidimo da je brojac uvecan lepo, da nema datarace slucajeva, ali je problem i pogresan nacin da u istoj iteraciji petlje kreiramo i pokrecemo nit,
pa onda kad to sve prodje u istoj iteraciji join-ujemo tu nit da njeno izvrsavanje saceka glavni program (main f-ja).
Zasto je ovo pogresno, pa zato sto se onda niti izvrsavaju sekvencijalno jedna za drugom umesto da rade u paraleli, sto je njihova poenta.
Jer ovde kakva je logika, prvo se jedna nit full izvrsi i zavrsi pa tek onda nastaje druga i tako u krug dok ne prodjemo kroz ceo niz thread handle-ova.

Ovakav program nema poente da koristi niti jer se one ovde izvrsavaju sekvencijalno a ne paralelno, sto nije njihova poenta.
*/