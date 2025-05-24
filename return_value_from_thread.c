#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

void* roll_dice() {
    int value = (rand() % 6) + 1;
    int* result = malloc(sizeof(int));
    *result = value;
    // printf("%d\n", value);
    printf("Thread took this memory address to write the result: %p\n", result);
    return (void*) result;	// nit vraca pokazivac na alociranu memoriju na hipu u kojoj se cuva rezultat bacanja kockice
}

int main() {
    int* res;
    srand(time(NULL));
    pthread_t th;
    
    if (pthread_create(&th, NULL, &roll_dice, NULL) != 0) {
        return 1;
    }
    if (pthread_join(th, (void**) &res) != 0) { 
        return 2;
    }
    printf("This is the memory address where the result is stored: %p\n", res);
    printf("Result: %d\n", *res);
    free(res);
    return 0;
}

/*
na adresu pokazivaca res u mainu koji ne pokazuje ni na sta, preko void**, odnosno preko njegove adrese, upisemo vrednost pokazivaca(pokazivaceva vrednost je adresa nekog podatka) u pokazivac res iz maina, tako da oba i res iz maina i result iz roll_dice() f-je
pokazuju na istu adresu alociranu na hipu, a na toj adresi se nalazi rezultat bacanja 

ovo se radi zato sto pthread_join() f-ja ocekuje kao drugi argument oblik: (void**)&ptr;


*/