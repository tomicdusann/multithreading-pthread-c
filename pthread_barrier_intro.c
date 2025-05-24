#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

#define THREAD_NUM 10

pthread_barrier_t barrier; // barijera je tip objekta koji zakljucava neki resurs sve dok se ne stvori dovoljan broj niti koje na njega cekaju


void* procedure(void* arg){
	int threadId = *((int*)arg);
	free(arg);

	/*stavljamo u petlju kako bi barijera bila uvek aktivna i kako ne bi zapucavao program ako broj niti koje pokusavaju da pristupe kodu koji "cuva" barijera nije umnozak broja niti koje barijera trazi da bi se "otvorila" */

	while(1){
		printf("[THREAD %d] Waiting for barrier to let me work...\n", threadId);
		pthread_barrier_wait(&barrier);  // "spustanje rampe dok nema odgovarajuceg broja niti koje cekaju"
		sleep(1);
		printf("[THREAD %d] Barrier let me in, doing work...\n", threadId);
		sleep(1);
		printf("[THREAD %d] Done with work.\n", threadId);
	}
}


int main(){
	pthread_t threads[THREAD_NUM];
	pthread_barrier_init(&barrier, NULL, 3);  // inicijlaizacija barijere - treci parametar je broj niti koje treba da cekaju da bi se ona otvorila
	
	for(int i=0; i<THREAD_NUM; i++){
		int* threadArg = (int*)malloc(sizeof(int));
		*threadArg = i;
		
		if(pthread_create(&threads[i], NULL, &procedure, threadArg) != 0){
			perror("[ERROR]: Failed createing a thread.\n");
		}
	}
	
	for(int i=0; i<THREAD_NUM; i++){
		if(pthread_join(threads[i], NULL) != 0){
			perror("[ERROR]: Failed to join a thread.\n");		
		}
	}
	
	pthread_barrier_destroy(&barrier);  // unistavanje barijere
	
	return 0;
}