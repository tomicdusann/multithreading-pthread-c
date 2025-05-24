#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>

#define THREAD_NUM 1

sem_t semNum;
pthread_mutex_t mutexNum;

int *number;

void* routine(void* args) {
    *number += 1;
    printf("Current value of number is %d\n", *number);
    sem_post(&semNum);
}

int main(int argc, char *argv[]) {
    pthread_t th[THREAD_NUM];
    
    number = malloc(sizeof(int));
    *number = 50;
    
    pthread_mutex_init(&mutexNum, NULL);
    sem_init(&semNum, 0, 0); // binarni semafor je semafor kao i svaku drugi samo sto moze imati 2 moguce vrednosti - 0 ili 1
    
    for (int i = 0; i < THREAD_NUM; i++) {
        if (pthread_create(&th[i], NULL, &routine, NULL) != 0) {
            perror("Failed to create thread");
        }
    }
    
    sem_wait(&semNum);
    printf("Deallocating number memory...\n");
    free(number);

    for (int i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
    
    pthread_mutex_destroy(&mutexNum);
    sem_destroy(&semNum);
    
    return 0;
}

/*
OTUPUT kada u kodu imamo implementiran semafor:
Current value of number is 51
Deallocating number memory...

OUTPUT kada u kodu nemamo implementiran semafor:
Deallocating number memory...
Current value of number is 50

Kada koristimo semafor mi osiguravamo da se zasigurno obezbedi da vrednost broja postane inkrementirana i time osiguravamo izvrsavanje koje smo zeleli
da postignemo. A kada semafora nema nama niko ne garantuje da ce se steci uslovi da OS dodeli resurse programu za kreiranje sporedne niti, a za to vreme
main nit moze da stignem da dealocira memoriju u kojoj cuvamo vrednost ciju modifikaciju treba da vrsi sporedna nit...

Main nit je oslobodila memoriju ali OS je još nije prepisao (srecan slucaj)
Kada pozovemo free(number), ta memorija je oznacena kao slobodna, ali njen sadrzaj (vrednost 50) moze ostati neko vreme u RAM-u dok operativni sistem ne odluzi da je pregazi novim zapisom.

Zato:
Sporedna nit moze da procita tu vrednost i napise "Current value is 50" — ali to je cista sreca.
Sledeći put moze da procita bilo koju vrednost.
*/