#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

#define THREAD_NUM 2

pthread_mutex_t mutexFuel;
pthread_cond_t condFuel;	// kondiciona varijabla deklaracija

int fuel = 0;

void* fuel_filling(void* arg){
	for(int i=0; i<5; i++){
		pthread_mutex_lock(&mutexFuel);
		fuel += 15;
		printf("[Fill thread]Filed fuel... %d.\n", fuel);
		pthread_mutex_unlock(&mutexFuel);
		pthread_cond_signal(&condFuel);  // signalizacija JEDNOJ cekajucoj niti, samo JEDNA nit ce se probuditi
										 // ako nijedna nit ne ceka signal se gubi
		sleep(1);	
	}
}

void* car(void* arg){
	pthread_mutex_lock(&mutexFuel);
	while(fuel < 40){	// provera uslova pre cekanja
		printf("[Car thread]No fuel. Waiting...\n");
		pthread_cond_wait(&condFuel, &mutexFuel);	// funkcija blokira nit sve dok se ne probudi(sve dok uslov while-a ne prestane da vazi)
		
		/*
			pthread_cond_wait(COND_VAR, MUTEX_VAR) radi sledeće:
           	1. Otključava `mutexFuel` i blokira nit dok ne primi signal.
           	2. Kada primi signal, ponovo zaključava `mutexFuel` pre nastavka rada.
           	3. Uslov `fuel < 40` se MORA ponovo proveriti nakon buđenja zbog spurious wakeup-a.
		*/
	}
	
	fuel -= 40;
	printf("[Car thread]Got fuel. Now left: %d.\n", fuel);
	pthread_mutex_unlock(&mutexFuel);
}

int main(){
	pthread_t threads[THREAD_NUM];
	pthread_mutex_init(&mutexFuel, NULL);
	pthread_cond_init(&condFuel, NULL);
	
	for(int i=0; i<THREAD_NUM; i++){
		if(i==1){
			if(pthread_create(&threads[i], NULL, &fuel_filling, NULL) != 0){
				perror("[ERROR]Failed to create a thread.\n");
			}
		} else{
			if(pthread_create(&threads[i], NULL, &car, NULL) != 0){
				perror("[ERROR]Failed to create a thread.\n");
			}
		}
	}
	
	for(int i=0; i<THREAD_NUM; i++){
		if(pthread_join(threads[i], NULL) != 0){
			perror("[ERROR]Failed to join a thread.\n");
		}
	}
	
	pthread_mutex_destroy(&mutexFuel);
	pthread_cond_destroy(&condFuel);
	
	return 0;
}