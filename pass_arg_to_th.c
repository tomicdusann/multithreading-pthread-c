#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define THREAD_NUM 10

int primeNums[THREAD_NUM] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};


void *procedure(void *arg){
	int primeIdx = *((int*)arg);
	printf("%d\n", primeNums[primeIdx]);
	free(arg);
}

int main(){
	pthread_t threads[THREAD_NUM];
	
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
	
	return 0;
}

/*
Poenta ovog koda je da se testira prosledjivanje argumenata kroz funkciju pthread_create(), gde argument mora da bude oblika (void*)adresa_argumenta, i u
takvom obliku treba da bude prijem argumenta za funkciju koju thread treba da izvrsava.

U ovom zadatku postoji niz prostih brojeva, gde dimenzija niza brojeva odgovara dimenziji niza niti. To omogucava da svaka nit uzme jedan broj iz niza i
ispise ga.

Nacin na koji se prosledjuju indeksi je takav sto se dinamicki zauzima memorija velicine int-a, a na adresu zauzete memorije se upisuje indeks elementa niza.
Taj indeks elementa niza je argument za nit, tako ce nit znati kom elementu da pristupi.

Zasto se upotrebljava dinamicki zauzeta memorija? Zato sto smo tada sigurni da se prilikom stvaranja niti i kad treba da se startuje nit, mi prosledjujemo 
zasigurno zakucanu vrednost u memoriji kao argument. Razlog toga je sto OS dodeljuje racunarske resrse za izvrsavanje niti, pa ako bi koristili int i, iterator 
for petlje, niko korisniku ne garantuje da ce se istog trenutka i pokrenuti ta nit za odgovarajuce i. Za to vreme dok kreirana nit ceka da se pokrene, preko i, main programska nit iterira kroz petlju i menja to i. To za naas znaci da ako recimo nasa prosla nit koja je kreirana za slucaj i=1, ako ne krene odmah da se izvrsava, a u medjuvremenu i postane 2, ono ce uzeti to novo i za argument, jer su se tek tad stekli uslovi da se nit pokrene.
I sad ako uzmemo recimo da nit koja je kreirana za slucaj i=1 i i=2 u istom trenutku dobiju zeleno svetlo za pokretanje, a i je jos uvek ostalo na 2,
one prakticno dobijaju isti argument i onda ne ispunjavamo uslov zadatka. 

Uslov glasi da svaka nit uzme po jedan razlicin broj iz skupa prostih brojeva, a mi bi u ovom hipotetickom primeru imali slucaj da 2 razlicite niti
donesu odaberu isti broj.
*/