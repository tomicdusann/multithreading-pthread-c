#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

void* roll_dice() {
    int value = (rand() % 6) + 1;
    int* result = malloc(sizeof(int));
    *result = value;
    sleep(2);
    printf("Thread result: %d\n", value);
    pthread_exit((void*) result);// return (void*)result; je ovde moglo da stoji i to je takodje ispravan nacin da vratimo vrednost iz niti - efekat je isti
}

int main(int argc, char* argv[]) {
    int* res;
    srand(time(NULL));
    pthread_t th;
    if (pthread_create(&th, NULL, &roll_dice, NULL) != 0) {
        return 1;
    }
     pthread_exit(0);
    
    // pthread_exit(0);  
    // Ako bismo ovde pozvali pthread_exit(0), to bi značilo da main nit završava svoje izvršavanje,  
    // ali pre nego što završi, omogućava svim drugim nitima unutar procesa da završe svoj posao.  
    // U ovom primeru nije potrebno, jer koristimo pthread_join(), koji već čeka da nit završi.
    
    if (pthread_join(th, (void**) &res) != 0) {
        return 2;
    }
    printf("Result: %d\n", *res);
    free(res);
    return 0;
}

/*
OUTPUT kada je "pthread_exit(0)" zakomentarisano:
Thread result: RANDOM_BROJ -> ispis u roll_dice niti
Result: RANDOM_BROJ -> ispis u main niti

OUTPUT kada je "pthread_exit(0)" otkomentarisan:
Thread result: RANDOM_BROJ -> imamo ispis samo roll_dice niti jer ce main na liniji "pthread_exit(0)" da stopira svoje izvrsavanja.
Zatim ce da saceka da se sporedna nit izvrsi (desice se ispis kao sastavni deo njenog izvrsavanja), a join f-je se nece desiti jer su one
deo koda koji dolazi na red nakon pthread_exit(0) komande.
*/