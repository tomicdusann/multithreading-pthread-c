#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>

#define THREAD_NUM 4 

typedef struct Task{
	int a, b;
} Task;

Task taskQueue[256];
int taskCount = 0; 	// number of tasks in queueu is 0 at the start

pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;


void executeTask(Task* task){
	usleep(50000); // vremenskim cekanjem simuliramo izvrsavanje nekog racunarski zahtevnog taska
	int result = task->a + task->b;
	printf("The sum of %d and %d is %d.\n", task->a, task->b, result);
}


void submitTask(Task task){
	pthread_mutex_lock(&mutexQueue);
	// kriticna sekcija
	taskQueue[taskCount] = task;  //dodavanje taska u queue
	taskCount++;	// azuriranje brojaca taskova
	// kriticna sekcija
	pthread_mutex_unlock(&mutexQueue);
	
	pthread_cond_signal(&condQueue); // s obzirom da jedna nit uzima jedan element koristimo signal a ne broadcast varijantu, da obavestimo samo jednu nit
}

// red je FIFO
// 1 2 3 4 5 red pre skidanja prvog
// 2 3 4 5   red posle skidanja prvog


void* startThread(void* args){
	while(1){
		Task task;
		
		pthread_mutex_lock(&mutexQueue); // pocetak kriticne sekcije -> fetch-ovanje taska iz queue-a 
		while(taskCount == 0){
			pthread_cond_wait(&condQueue, &mutexQueue); //pthread_cond_wait automatski otpusta mutex dok ceka i ponovo ga zakljucava kad se probudi
		}
		
		task = taskQueue[0];
		for(int i = 0; i < taskCount - 1; i++) {
			taskQueue[i] = taskQueue[i + 1];
		}
		taskCount--;
		pthread_mutex_unlock(&mutexQueue); // kraj kriticne sekcije -> proces fetch-ovanja je sada zavrsen
		
		executeTask(&task);
	}
}

int main(){
	pthread_t threads[THREAD_NUM];
	
	pthread_mutex_init(&mutexQueue, NULL);
	pthread_cond_init(&condQueue, NULL);
	
	for (int i = 0; i < THREAD_NUM; i++) {
        	if (pthread_create(&threads[i], NULL, &startThread, NULL) != 0){
            		perror("Failed to create the thread");
        	}
    	}
	
	for(int i = 0; i < 100; i++) {
		Task t = {
			.a = rand() % 100,
			.b = rand() % 100
		};
		submitTask(t);
	}
	
	for (int i = 0; i < THREAD_NUM; i++) {
        	if (pthread_join(threads[i], NULL) != 0) {
            		perror("Failed to join the thread");
        	}
    	}
    	
    	pthread_mutex_destroy(&mutexQueue);
    	pthread_cond_destroy(&condQueue);
    	
    	return 0;
}

/*
Thread Pool je unapred kreirani skup niti koje cekaju da dobiju zadatke koje treba da izvrse, umesto da se za svaki zadatak kreira nova nit. Koristi se kada imamo veliki broj kratkih zadataka koje treba brzo i efikasno obraditi, bez stalnog trosenja resursa na pravljenje i unistavanje niti.
*/