#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

// chefs = threads
// stove = shared data (+mutex)

pthread_mutex_t stoveMutex[4];
int stoveFuel[4] = { 100, 100, 100, 100 };	// 4 peci, za svaku pec po mutex

void* routine(void* args) {
    for (int i = 0; i < 4; i++) {
        if (pthread_mutex_trylock(&stoveMutex[i]) == 0) {
            int fuelNeeded = (rand() % 30);
            if (stoveFuel[i] - fuelNeeded < 0) {
                printf("No more fuel... going home\n");
            } else {
                stoveFuel[i] -= fuelNeeded;
                usleep(500000);
                printf("Fuel left %d\n", stoveFuel[i]);
            }
            pthread_mutex_unlock(&stoveMutex[i]);
            break;
        } else {   		// ako nit ne moze da uzme mutex i dosli smo do poslednje iteracije, stavi iteracioni korak na pocetak i krenu ponovo 
            if (i == 3) {
                printf("No stove available yet, waiting...\n");
                usleep(300000);
                i = 0;		// resetovanje iteratora
            }
        }
    }
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    pthread_t th[10];
    for (int i = 0; i < 4; i++) {
        pthread_mutex_init(&stoveMutex[i], NULL);
    }
    for (int i = 0; i < 10; i++) {
        if (pthread_create(&th[i], NULL, &routine, NULL) != 0) {
            perror("Failed to create thread");
        }
    }

    for (int i = 0; i < 10; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
    for (int i = 0; i < 4; i++) {
        pthread_mutex_destroy(&stoveMutex[i]);
    }
    return 0;
}

/*
Ovaj kod demonstrira praktičnu upotrebu funkcije pthread_mutex_trylock() za sinhronizaciju niti pomoću mutex promenljivih.

Imamo 10 niti koje simuliraju kuvare i 4 mutex promenljive koje kontrolišu pristup šporetima.  
Koristimo trylock() zato što je neblokirajuća funkcija – ako mutex nije dostupan, nit može odmah pokušati da zaključa neki drugi mutex  
ili sačekati kratko pre ponovnog pokušaja.

Za razliku od pthread_mutex_lock(), koja blokira nit dok mutex ne postane slobodan, trylock() omogućava brže izvršavanje  
jer niti ne moraju čekati na jedan određeni resurs, već mogu tražiti slobodan šporet.  
Na taj način smanjujemo vreme čekanja i poboljšavamo efikasnost programa.
*/
