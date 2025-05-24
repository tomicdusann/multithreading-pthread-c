#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

int counter = 0;

void* procedure(){
	for(int i=0; i<1000000; i++){
		counter++;
	}
}

int main(){
	pthread_t p1, p2;

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

	printf("Counter value: %d \n", counter);

	return 0;
}

/*
Prilikomo svakog ponovnog izvrsavanja koda vrednost pbrojaca ce biti drugacija.

Sama po sebi increment operacija ++ nije atomska sto znaci da se na nivou masinskog jezika i asemblera ne odvija u jednom koraku.
Potrebno je:
- ucitati vrednost iz memorije (RAM ili cache mem.);
- uvecati vrednost;
- ucitati nazad vrednost u memoriju (RAM ili cache mem.).

Zasto je to tako? U sustini kad jedna nit vrsi inkrement ona moze da pod znacima navoda uvecava promenljivu i da je sve vreme prolaska kroz petlju drzi u nekom kesu ili u registrima procesora zbog optimizacije, a moze u bilo kom trenutku da je upise nazad u RAM a i ne mora. Za to vreme inicijalizovace se i pokrenuti druga nit koja ce, s obzirom da deli isti adresni prostor i podatke kao i prva nit, moci da pristupi trenutnoj vrednosti promenljive u RAM-u, bez obzira da li je ta vrednost skoro azurirana od strane prosle niti ili je ostala ista kao kad je toj promenljivoj pristupila ona prva nit.

Ovo zavisi od mnogo faktora. Ali bitno je zapamtiti da na nivou kompajlera je da se obezbedi da niti imaju isti adresni prostor i podatke kojima treba da pristupaju, ako je predvidjeno da im obe pristupaju. Kako ce se samo azuriranje i obezbedjivanje tacnih vrednosti za prodatak i da li ce biit njegovog "dupliranja" i rasprostiranja u vise cache novao, to zavisi od procesora i njegove arhitekture, odnosno hardverske izvedbe neke generacije procesora.

U sustini veliki zakljucak bi bio da mi kao programeri ne treba puno da razmisljamo kako je to reseno u pogledu hardverske arhitekture, treba da imamo povrenje u kompajler i OS da ce adekvatno da podele adresni prostor procesa kako bi niti u okviru njega mogle da komuniciraju i obradjuju podatke kako treba. Na nama je da koristimo odgovarajuce sinhronizacione mehanizme i obezbedimo da ne dodje ni do kakvog data race scenarija koji bi mogao da dovede nas softver u nekonzistentno stanje.

Sto se hardverske strane tice, datarace moze da se desi samo ako posedujete multicore/multithread procesor, jer jedino na njemu mozemo imati vise od 1 aktivnog threada odnosno niti koje trce u jednom trenutku, npr. na 4 fizicke niti trce istovremeno 4 softverske niti i tada je datarace moguce. A ako imamo jedno 
CPU jezgro uvek ce procesorsko vreme i kvantum da se naizmenicno predaje 1. 2. 3. i 4. niti, odnosno u jednom trenutku takav procesor ce simultano izvrsavati samo 1 nit, a ostale ce morati da budu u fazi cekanja.

*/