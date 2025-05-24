#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<semaphore.h>

#define THREAD_NUM 4

sem_t semaphore;   // deklaracija podatka strukture sem_t, koja predstavlja semafor

void* procedure(void* args){
	sem_wait(&semaphore); 	// nit uzima propusnicu od semafora da odradi posao
	sleep(1);
	printf("Hello from [THREAD %d].\n", *((int*)args));
	sem_post(&semaphore);	// nit vraca propusnicu nazad nakon zavrsenog posla
	free(args);
}

int main(){
	pthread_t threads[THREAD_NUM];
	
	sem_init(&semaphore, 0, 2);	// inicijalizacija semafora
	
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
	
	sem_destroy(&semaphore);	// unistavanje semafora - oslobadnjanje sistemskih resursa pridruzenih semaforu
	
	return 0;
}


/*
	sem_init(parametar1, parametar2, parametar3):
	1. parametar1 - pokazivac na strukturu sem_t
	2. parametar2 - moze biti ili 0 ili 1; 0 - imamo jedan proces i u njemu vise niti; 1- imamo vise procesa koji koriste semafor
	3. parametar3 - integer value koji je u sustini "propusnica", celobrojna vrednost koja govori koliko niti moze paralelno da izvrsava kritican deo koda
	
	sem_wait() je slicno kao mutex_lock(). Preko ove funkcije cekamo da nam semafor da propusnicu da odradimo zadatak, a po zavrsetku posla
	radi se nesto analogno mutex_unlock() a to je f-ja sem_post(), kojom vracamo propusnicu nazad semaforu.
	Ta "propusnica" je integer polje sem_t strukture koje daje informaciju koliko niti semafor moze da propusti istovremeno. Vrednost se smanjuje
	za 1 svakim zauzecem semafora i povecava za 1 nakon svakog post().
	Ukoliko je trenutna vrednost "propusnice" 0, to znaci da je semafor zauzet totalno i nit/niti moraju da cekaju da vrednost bude >0 kako bi
	neka od njih mogla da pristupi kriticnom kodu ciji pristup cuva semafor.
	
	Ocekivani output:
	__ispis niti__
	__ispis niti__
	pauza(1sec)
	__ispis niti__
	__ispis niti__
	
	U ovom zadatku semafor propusti odjednom 2 niti, gde svaka odradi sleep(1) i ispis istovremenom, zatim oslobode semafor, 
	pa ga zauzmu druge 2 niti i ponove postupak, a ovaj sleep(1) nam sustinski sluzi da mozemo lakse da razaznamo vremensku distancu izmedju 
	zauzimanja semafora od prve 2 niti i druge 2 niti, jer ovaj semafor ide po principut 2 po 2 niti.
	
*/	