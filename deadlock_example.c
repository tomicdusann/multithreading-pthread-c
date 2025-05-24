#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define THREAD_NUM 2

pthread_mutex_t mutexApples;
int apples = 50;
pthread_mutex_t mutexBananas;
int bananas = 10;

void* routine(void* args) {
    if (rand() % 2 == 0) {
        pthread_mutex_lock(&mutexApples);
        sleep(1);	// da damo dovoljno vremena da dok jedna nit zauzme jedan mutex da druga zauzme drugi, kako bi se videlo gde nastaje konflikt(deadlock)
        pthread_mutex_lock(&mutexBananas);
    } else {
        pthread_mutex_lock(&mutexBananas);
        sleep(1);
        pthread_mutex_lock(&mutexApples);
    }

    apples += 50;
    bananas = apples;
    
    printf("Incremented fuel to: %d set water to %d\n", apples, bananas);
    
    pthread_mutex_unlock(&mutexApples);
    pthread_mutex_unlock(&mutexBananas);
}

int main() {

    pthread_t th[THREAD_NUM];
    pthread_mutex_init(&mutexApples, NULL);
    pthread_mutex_init(&mutexBananas, NULL);
    
    for (int i = 0; i < THREAD_NUM; i++) {
        if (pthread_create(&th[i], NULL, &routine, NULL) != 0) {
            perror("Failed to create thread");
        }
    }

    for (int i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
    
    printf("Fuel: %d\n", apples);
    printf("Water: %d\n", bananas);
    pthread_mutex_destroy(&mutexApples);
    pthread_mutex_destroy(&mutexBananas);
    return 0;
}

/*
Poenta je da na neki nacin simuliramo deadlock, odnosno mrtvu petlju - nastanak medjuzavisnosti koji je takav da zaustavi ceo program i da on beskonacno ceka.
Ovde ce jedna od niti zauzeti mutex za jabuke, a druga za banane.
Nakon sto svaka nit zauzme po jedan mutex pokrenut je sleep(1) na 1 sekundu, kako bi se ostvario vremenski prostor dovoljan da se pokrene i druga nit 
i da ona takodje zauzme svoj mutex, koji treba da je suprotan mutex-u koji zauzima prethodno pokrenuta nit (ako je nit1 zauzela jabuke mutex, ova ce zauzeti 
banane mutex).
Kada se ovo desi svaka od niti treba da zauzme i drugi mutex, ali posto je taj koji joj preostaje da zauzme vec prethodno uzet od druge niti, nastace
beskonacno cekanje i medjuzavisnost koja nikada nece prestati, cime nas program ulazi u mrtvu petlju.

Jedna nit je zauzela banane mutex i ceka da zauzme jabuke mutex, ali ne moze jer je druga nit zauzela jabuke mutex, a da bi ga oslobodila mora da stigne 
do tog dela izvrsavanja koda, a ne moze da dodje ni do tog dela jer prethodno treba da zauzme i banana mutex.
Ovo je medjuzavisnost koja "tera" obe niti da se vrte u beskonacnosti jer uslovi da nastave svoja izvrsavanja nikada nece nastupiti.
*/