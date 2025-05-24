#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define THREAD_NUM 2
#define ARR_LEN 10

int arrayNums[ARR_LEN] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};

void* sumNums(void* arg){
	int* arrStartIndex;
	arrStartIndex = (int*)arg;
	
	int* sum = (int*)malloc(sizeof(int));		// za sumu koju racuna svaka nist alociramo memoriju za storovanje na hipu
	*sum = 0;
	
	for(int i=0; i<(ARR_LEN/2); i++){
		*sum += arrayNums[*arrStartIndex + i];
	}
	
	free(arg);		// dinamicko oslobadjanje memorije posto smo preko prosledjenog argumenta odradili ono sto je trebalo u f-ji
				// pa nam argument vise ne treba
	
	return (void*)sum;				// kao porvratnnu vrednost funkcije samo vratimo pokazivac na zauzetu memoriju
}

int main(){
	pthread_t threads[THREAD_NUM];
	int bigSum = 0;
	
	for(int i=0; i<THREAD_NUM; i++){
		int* thArg = (int*)malloc(sizeof(int));
		*thArg = i * 5;
		
		if(pthread_create(&threads[i], NULL, &sumNums, (void*)thArg) != 0){
			perror("[ERROR]Failed to create a thread.\n");
		}
	}
	
	for(int i=0; i<THREAD_NUM; i++){
		int* sumResult;
	
		if(pthread_join(threads[i], (void**)&sumResult) != 0){	// prazan pokazivac(neinicijalizovan pocetnom vrednoscu ) sumResult dobija vrednost 										adrese u memoriji koja cuva izracunatu sumu  
			perror("[ERROR] Could not join the thread.\n");
		}
		
		bigSum += *((int*)sumResult);
		
		free(sumResult);		// dinamicko oslobadjanje memorije koju smo zauzeli da smestimo rezultat sabiranja preko niti
						// vrednost sa te memoriske lokacije dodelili smo ukupnoj sumi i obavestavamo OS da mu prepustamo tu mem.
	}
	
	printf("SUM of all array numbers is %d.\n", bigSum);
	
	return 0;
}

/*
- kad saljemo argumente niti pozeljno je da ih eksplicitno i kastujemo u oblik koji funkcije ocekuju
- isto to je vazno da uradimo i kada primamo povratnu vrednost funkcije koju neka nit izvrsava
*/
