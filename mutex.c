/*
Zamislimo da imamo neku globalnu varijablu int lock = 0;

I sad neki kod koji je rizican po tome sto mu pristupa vise niti odjrednom ili pokusava da mu pristupi vise niti odjednom, one mogu preko tog dela koda
da dovedu odredjene podatke i sam program u nekonzistentno stanje, sto nikada nije dobro u situacijama kada zelimo da tacno sve ima svoj redosled.

E sad zamislimo da smo taj kod nekako uokvirili, da pre i posle tog kriticnog dela koda imamo proveru stanja lock-a.

Nesto ovako npr:

lock = 1; // postavljamo lock na 1 kada hocemo da zakljucamo kod, prisvojimo ga trenutnoj niti da druge ne bi mogle da ga izvrse

KRITICAN_KOD

lock = 0; // kad nit izvrsi to sto treba sa njim lock vise ne vazi i toj kriticnoh sekciji koda moze da pristupi neka druga nit


Ali kako ce neka druga nit da zna da je lock aktiviran, tako je, provericemo uslovom da li je lock 1 (aktivan), a ako jeste, nit ce cekati.
Pa bi pseudokod izgleda ovako:

int lock = 0;
.
.
.

if(lock == 1)
	//wait until lock = 0

lock = 1

KRITICAN_KOD

lock = 0
	
Da ne bi pravili rucno varijablu sa ovakvom nekom ulogom, POSIX thread biblioteka ima ugradjenu promenljivu MUTEX, koja sustinski radi sve ovo sto sam gore 
nadam se objasnio. MUTEX je jedan od mehanizama sinhronizacije niti, koji sprecava datarace slucajeve i omogucava kontrolisan pristup i redosled koriscenja
resursa i koda programa od strane niti, cime se poboljsava kontrola izvrsavanja i program se ne dovodi u nekonzistentno stanje.
*/

#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>

pthread_mutex_t mutex;
int counter = 0;

void* procedure(){
	for(int i=0; i<1000000; i++){
		pthread_mutex_lock(&mutex);  	// if(lock == 1) wait until lock = 0; lock = 1 -> provera i postavljanje na 1 sve odjednom radi ova f-ja
		counter++;						// KRITICAN_DEO_KODA
		pthread_mutex_unlock(&mutex); 	// lock = 0
	}
}

int main(){
	pthread_t p1, p2;
	pthread_mutex_init(&mutex, NULL);  // inicijalizacija mutex-a sa default ponasanjem(NULL)


	if(pthread_create(&p1, NULL, &procedure, NULL) != 0){
		return 1;		
	}

	if(pthread_create(&p2, NULL, &procedure, NULL) != 0){
		return 2;	
	}

	if(pthread_join(p1, NULL) != 0){
		return 3;
	}

	if(pthread_join(p2, NULL) != 0){
		return 4;
	}

	pthread_mutex_destroy(&mutex);	// dealokacija memorije zauzete varijablom lock-a

	printf("Counter value: %d \n", counter);

	return 0;
}

/*
OUTPUT: 2000000

Ispis je sada uvek tacan, jer jedna nit uveca counter tacno milion puta, zatim druga nit uradi to isto za jos milion puta, pa imamo 2mil. 
*/
