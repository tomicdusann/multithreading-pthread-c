#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

void* procedure(void* thread_id){	//Povratna vrednost je void* jer nit može vratiti bilo koji tip podataka koristeći pokazivač
					//Argument je isto tipa void* da bi nit (njena funkcija) mogla da primi bilo sta, pa se kasnije kastuje
	int id = *((int*)thread_id);
	
	if(id == 1){
		printf("Hello from thread 1.\n");
		sleep(3);
		printf("Ending thread 1.\n");
	}
	
	if(id == 2){
		printf("Hello from thread 2.\n");
		sleep(3);
		printf("Ending thread 2.\n");
	}	
}

int main(){

	pthread_t p1, p2;
	int id1, id2;
	
	id1 = 1;
	id2 = 2;
	
	if(pthread_create(&p1, NULL, &procedure, &id1) != 0){
		return 1;
	}
	
	if(pthread_create(&p2, NULL, &procedure, &id2) != 0){
		return 2;
	}
	
	if(pthread_join(p1, NULL) != 0){
		return 3;
	}
	
	if(pthread_join(p2, NULL) != 0){
		return 4;
	}
	
	return 0;

}

/*
Ispisi mogu biti drugaciji od slucaja do slucaja, nekad ce se prvo javiti prva nit, nekad druga, isto vazi i za odjavu niti.
To je tako jer OS(scheduler) dodeljuje procesorske resurse nitima.
Ukoliko imamo multicore/multithread procesore sa recimo 8 threadova, to znaci da mozemo izvrsavati u jednom trenutku simultano 8 softverskih thredova(niti).
sleep() je funkcija koja odlaze izvrsavanje niti dalje, ali ona kao takva ne garantuje da ce nit odmah po isteku intervala nastaviti izvrsavanje.
*/
