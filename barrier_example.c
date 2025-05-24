#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define THREAD_NUM 8	// broj niti radnika

int dice_values[THREAD_NUM];
int status[THREAD_NUM] = { 0 };

pthread_barrier_t barrierRolledDice;
pthread_barrier_t barrierCalculated;

void* roll(void* args) {
    int index = *(int*)args;
    
    while (1) {	// da se vrti beskonacno
        dice_values[index] = rand() % 6 + 1;
        pthread_barrier_wait(&barrierRolledDice);
        pthread_barrier_wait(&barrierCalculated);
        if (status[index] == 1) {
            printf("([THREAD %d] rolled %d) I won\n", index, dice_values[index]);
        } else {
            printf("([THREAD %d] rolled %d) I lost\n", index, dice_values[index]);
        }
    }
    
    free(args);
}

int main() {
    srand(time(NULL));
    pthread_t th[THREAD_NUM];
    
    int i;
    
    pthread_barrier_init(&barrierRolledDice, NULL, THREAD_NUM + 1);	// +1 se odnosi na main nit -> da bi se izvrsavao kod koju "cuvaju" barijere, sve worker 
    									// niti kao i main nit moraju da pozovu wait() za barijeru
    									
    pthread_barrier_init(&barrierCalculated, NULL, THREAD_NUM + 1);	// +1 se i ovde odnosi na main nit
    
    for (i = 0; i < THREAD_NUM; i++) {
        int* a = malloc(sizeof(int));
        *a = i;
        
        if (pthread_create(&th[i], NULL, &roll, (void*)a) != 0) {
            perror("Failed to create thread");
        }
    }

    while (1) {
        pthread_barrier_wait(&barrierRolledDice);
        // Calculate winner
        int max = 0;
        for (i = 0; i < THREAD_NUM; i++) {
            if (dice_values[i] > max) {
                max = dice_values[i];
            }
        }

        for (i = 0; i < THREAD_NUM; i++) {
            if (dice_values[i] == max) {
                status[i] = 1;
            } else {
                status[i] = 0;
            }
        }
        
        sleep(1);
        
        printf("==== New round starting ====\n");
        pthread_barrier_wait(&barrierCalculated);
    }

    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
    pthread_barrier_destroy(&barrierRolledDice);
    pthread_barrier_destroy(&barrierCalculated);
    return 0;
}


/*
Kad main nit pozove da ceka sa barrier wait() za rollanje kockice, ona ceka da sve niti odrade roll i pozovu se i one na wait za tu barrijeru. To znaci da je su u tom trenutku sve niti radnici bacile kockicu, i sad treba main nit da odradi svoj deo posla i nadje maksimalnu vrednost bacanja od svih bacanja radnickih niti.
U ovom momentu uslov za sklanjanje barijere je ispunjen jer su sve niti radici (njih ukuono THREAD_NUM) bacili kockicu i pozvali se na wait za barijeru
barrierRolledDice, a to je uradila i main nit, sto znaci da je uslov za broj niti koje sklanjaju barijeru iz inicijalizacije THREAD_NUM+1 ispunjen.
Kod iza barijere je sada dostupan i main moze da odradi svoj deo posla.
Kad main nit to sve odradi(nadje maksimalnu vrednost bacanja od svih bacanja radnickih niti) i  na osnovu toga dodeli statuse bacanjima, na osnovu kojih kasnije svaka nit ispisuje njen status pobeda ili poraz.
Onda main nit ispise da krece pocetak nove runde i pozove wait za barijeru calculated.
A kako svaka radnicka nit pozove wait i za barijeru rolledDice i za Calculated, to znaci da i one zajedno sa main niti cekaju wait na barrierCalculated-u. To znaci da opet imamo odgovarajuci broj niti koje cekaju i onda svaka nit moze da ispise svoj status, nakon cega kad ispise status opet krece pozivanje barijera i p postupak se ponavlja.
*/


/*
Barijera NIJE kao mutex – niti koje cekaju na barijeru se "pustaju" sve odjednom, i tada sve paralelno nastavljaju svoje izvrsavanje nakon sto barijera bude "probijena" (kada ih dovoljno stigne do tog mesta). Znasi, ne čekaju jedna drugu da zavrsi kod nakon barijere - izvrsavaju se paralelno, naravno ukoliko
nam arhitekture procesora i sam CPU cip podrzavaju multicore/multithread arhitekturu i tehnologiju(vrsenje zadataka u paraleli a ne konkurentno smenjivanje
jedne po jedne softverske niti na jednom fizickom jezgru/threadu).
*/