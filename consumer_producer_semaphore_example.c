#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<time.h>

#define THREAD_NUM 8

sem_t semEmptyBuffer;
sem_t semFullBuffer;

pthread_mutex_t mutexBuffer;

int buffer[10]; // bafer ima max 10 elemenata koje moze da primi u sebe
int elementsInBuff = 0;	// inicjalno je bafer prazan

void* producer(void* args){
	while(1){
		int x = rand() % 100;
		sleep(1);
		
		
		sem_wait(&semEmptyBuffer); // uzima propusnicu, vrednost semafora se smanjuje, odnosno on ima sve manje slobodnih slotova -> nit stvara element
		
		pthread_mutex_lock(&mutexBuffer); // nit se zakljucava na bafer kako bi dodala element
		buffer[elementsInBuff] = x; //__KRITICNA_SEKCIJA__
		elementsInBuff++;           //__KRITICNA_SEKCIJA__
		pthread_mutex_unlock(&mutexBuffer); // odvezuje se od bafera da mogu da mu prisupe druge niti 
		
		sem_post(&semFullBuffer); // inkrementira se propusnica na ovom semaforu, sto znaci da u njemu sada ima elemenata i to znaci da conusmer
					 // ima sta da preuzme iz bafera -> inkrementira se propusnica kako bi consumer mogao da je uzme i da preuzme broj
	}
}

void* consumer(void* args){
	while(1){
		int y;	
		
	
		sem_wait(&semFullBuffer);  // consumer uzima propusnicu kako bi mogao da preuzme propusnicu koju je producer nit generisala 
		
		pthread_mutex_lock(&mutexBuffer);
		// bafer je LIFO struktura 
		y = buffer[elementsInBuff - 1];	//__KRITICNA_SEKCIJA__
		elementsInBuff--; 		//__KRITICNA_SEKCIJA__
		pthread_mutex_unlock(&mutexBuffer);
		
		sem_post(&semEmptyBuffer); // inkrementira vrednost za ovaj semafor sto znaci da je element uzet iz "slota" i niti radnici mogu da generisu novi
		
		printf("[CONSUMER]: Got %d.\n", y);
		
		sleep(1);
	}
}

int main(){
	srand(time(NULL));
	
	pthread_t threads[THREAD_NUM];
	pthread_mutex_init(&mutexBuffer, NULL);
	
	sem_init(&semEmptyBuffer, 0, 10); // semafor simulira stanje praznog buffera, tada je vrednost propusnice 10 -> implicira velicinu bafera na 10 elemnata
	sem_init(&semFullBuffer, 0, 0);	// semafor simulira stanje punog buffera, tada je vrednost propusnice 0 -> 
	
	for(int i=0; i<THREAD_NUM; i++){
		if(i>0){
			if(pthread_create(&threads[i], NULL, &producer, NULL) != 0){
				perror("Failed to create a thread.\n");
			}
		} else{
			if(pthread_create(&threads[i], NULL, &consumer, NULL) != 0){
				perror("Failed to create a thread.\n");
			}
		}
	}
	
	for(int i=0; i<THREAD_NUM; i++){
		if(pthread_join(threads[i], NULL) != 0){
			perror("Failed to join a thread.\n");
		}
	}
	
	sem_destroy(&semEmptyBuffer);
	sem_destroy(&semFullBuffer);
	
	pthread_mutex_destroy(&mutexBuffer);
	
	return 0;
}