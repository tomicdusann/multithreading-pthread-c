#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

#define THREAD_NUM 3

pthread_mutex_t mutex0;
pthread_mutex_t mutex1;

void* procedure0(void* arg){
	int id = *((int*)arg);
	free(arg);
	
	pthread_mutex_lock(&mutex0);
	printf("[PROCEDURE0 #TH_ %d]Got lock\n", id);
	sleep(1);
	pthread_mutex_unlock(&mutex0);
}

void* procedure1(void* arg){
	int id = *((int*)arg);
	free(arg);

	if(pthread_mutex_trylock(&mutex1) == 0){
		printf("[PROCEDURE1 #TH_ %d]Got lock\n", id);
		sleep(1);
		pthread_mutex_unlock(&mutex1);
	} else{
		printf("[PROCEDURE1# TH_ %d]Can't get lock.\n", id);
	}
}

int main(){
	pthread_t threads0[THREAD_NUM];
	pthread_t threads1[THREAD_NUM];

	pthread_mutex_init(&mutex0, NULL);
	pthread_mutex_init(&mutex1, NULL);
	
	for(int i=0; i<THREAD_NUM; i++){
		int* thArg = (int*)malloc(sizeof(int));		// uvek ovako dinamicki zauzimati memoriju za argumente da budemo sigurni sta prosledjujemo 
		*thArg = i;
		
		if(pthread_create(&threads0[i], NULL, &procedure0, (void*)thArg) != 0){
			perror("[ERROR] Could not create a thread.\n");
		}
	}
	
	for(int i=0; i<THREAD_NUM; i++){
		if(pthread_join(threads0[i], NULL) != 0){
			perror("[ERROR] Could not join a thread.\n");
		}
	}
	
	for(int i=0; i<THREAD_NUM; i++){
		int* thArg = (int*)malloc(sizeof(int));
		*thArg = i;
		
		if(pthread_create(&threads1[i], NULL, &procedure1, (void*)thArg) != 0){
			perror("[ERROR] Could not create a thread.\n");
		}
	}
	
	for(int i=0; i<THREAD_NUM; i++){
		if(pthread_join(threads1[i], NULL) != 0){
			perror("[ERROR] Could not join a thread.\n");
		}
	}
	
	pthread_mutex_destroy(&mutex0);
	pthread_mutex_destroy(&mutex1);
	
	return 0;
}

/*
Poenta ovog koda je da prikaze razliku izmedju funkcija lock() i trylock() u oktivu pthread.h biblioteke.
Glavna razlika je u tome sto trylock() pokusava jednom da uzme lock i ako ne uspe vise nece pokusavati. 
Za razliku od nje, f-ja lock() pored toga sto pita da li je lock slobodan ona ceka sve dok se lock ne oslobodi i onda ga uzima.
*/
