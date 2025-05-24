#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define THREAD_NUM 2

void* routine(void* args) {
    sleep(1);
    printf("Finished execution\n");
}

int main() {
    pthread_t th[THREAD_NUM];
    
    pthread_attr_t detachedThread;
    pthread_attr_init(&detachedThread);
    pthread_attr_setdetachstate(&detachedThread, PTHREAD_CREATE_DETACHED);
    
    for (int i = 0; i < THREAD_NUM; i++) {
        if (pthread_create(&th[i], &detachedThread, &routine, NULL) != 0) {
            perror("Failed to create thread");
        }
        // pthread_detach(th[i]);
    }

    for (int i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
    
    pthread_attr_destroy(&detachedThread);
    pthread_exit(0);	// Glavna nit se završava, ali proces ostaje živ dok sve ostale niti ne završe.
			// detached niti ce imati vremena da odrade svoj posao i ispišu "Finished execution"

}

/*
    Ovaj kod omogucava da niti budu sigurno kreirane u detached rezimu;
    Kada su niti detached, one se ne mogu join-ovati iz glavne niti;
    Sistem automatski oslobađa njihove resurse kada se zavrse – nije potrebno rucno unistavanje;
    
    Glavna nit moze da pozove pthread_exit(0), cime se ona sama zavrsava,
    ali proces ostaje aktivan dok se sve ostale niti (ukljucujuci detached) ne zavrse;
    
    Detached niti su korisne kada zelimo da rade nezavisno i ne treba da ih sinhronizujemo sa glavnom niti;

*/