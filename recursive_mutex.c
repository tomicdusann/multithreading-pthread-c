#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define THREAD_NUM 8

pthread_mutex_t mutexFuel;
int fuel = 50;

void* routine(void* args) {
    pthread_mutex_lock(&mutexFuel);
    pthread_mutex_lock(&mutexFuel);
    pthread_mutex_lock(&mutexFuel);
    pthread_mutex_lock(&mutexFuel);
    pthread_mutex_lock(&mutexFuel);
    
    fuel += 50;
    printf("Incremented fuel to %d\n", fuel);
    
    pthread_mutex_unlock(&mutexFuel);
    pthread_mutex_unlock(&mutexFuel);
    pthread_mutex_unlock(&mutexFuel);
    pthread_mutex_unlock(&mutexFuel);
    pthread_mutex_unlock(&mutexFuel);
}

int main() {
    pthread_t th[THREAD_NUM];
    
    pthread_mutexattr_t recursiveMutexAttributes;
    pthread_mutexattr_init(&recursiveMutexAttributes);
    pthread_mutexattr_settype(&recursiveMutexAttributes, PTHREAD_MUTEX_RECURSIVE);
    
    pthread_mutex_init(&mutexFuel, &recursiveMutexAttributes);
    
    int i;
    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_create(&th[i], NULL, &routine, NULL) != 0) {
            perror("Failed to create thread");
        }
    }

    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
    
    printf("Fuel: %d\n", fuel);
    
    pthread_mutexattr_destroy(&recursiveMutexAttributes);
    pthread_mutex_destroy(&mutexFuel);
    
    return 0;
}

/*
rekurzivni mutex se pomocu mutexattr_t atributa inicijalizuje kao rekurzivni i omogucava da se jedan mutex vise puta zakljuca 
od strane jedne niti - ovo je omoguceno sve dok je broj zakljucavanja isti kao i broj otkljucavanja

bukvalno kao da tako sto vise puta sa nekom niti zakljucamo mutex njemu dodajemo brojac koji odgovara broju zakljucavanja, a taj brojac se 
smanjuje otkljucavanjem

kada taj brojac dodje na 0 to znaci da je nit pravilno otkljucala mutex i sada moze da ga preuzme neka druga nit
*/