#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>

#define THREAD_NUM 16

sem_t semaphore;

void* procedure(void* args){
	printf("[THREAD %d] Waiting in the login queue\n", *((int*)args));
	
	sem_wait(&semaphore);
	printf("[THREAD %d] Logged in\n", *((int*)args));
	sleep(rand() % 5 + 1);
	printf("[THREAD %d] Logged out\n", *((int*)args));
	sem_post(&semaphore);
	
	free(args);
}

int main(){

	pthread_t threads[THREAD_NUM];
	
	sem_init(&semaphore, 0, 12);
	
	for(int i=0; i<THREAD_NUM; i++){
		int* a = (int*)malloc(sizeof(int));
		*a = i;
		
		if(pthread_create(&threads[i], NULL, &procedure, (void*)a) != 0){
			perror("Failed to create a thread.\n");
		}
	}
	
	for(int i=0; i<THREAD_NUM; i++){
		if(pthread_join(threads[i], NULL) != 0){
			perror("Failed to join threads.\n");
		}
	}

	sem_destroy(&semaphore);

	return 0;
}