#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define THREAD_NUM 2

typedef enum operation{
	SABIRANJE,
	MNOZENJE
} Operation;

typedef struct Task {
    void (*taskFunction)(int*, int*, int*);
    int arg1, arg2, result;
    Operation operationType;
} Task;

Task taskQueue[256];
int taskCount = 0;

Task completedTasksQueue[256];
int completedTasksCount = 0;

pthread_mutex_t mutexQueue;
pthread_mutex_t mutexCompletedQueue;
pthread_cond_t condQueue;
pthread_cond_t condCompletedQueue;

void sum(int* arg1Address, int* arg2Address, int* resultAddress) {
    usleep(50000);
    *(resultAddress) = *(arg1Address) + *(arg2Address);
}

void product(int* arg1Address, int* arg2Address, int* resultAddress) {
    usleep(50000);
    *resultAddress = (*(arg1Address)) * (*(arg2Address));
}

void submitCompletedTask(Task* task){
    pthread_mutex_lock(&mutexCompletedQueue);
    completedTasksQueue[completedTasksCount] = *task;
    completedTasksCount++;
    pthread_mutex_unlock(&mutexCompletedQueue);
    pthread_cond_signal(&condCompletedQueue);
}

void executeTask(Task* task) {
    task->taskFunction(&(task->arg1), &(task->arg2), &(task->result));
    submitCompletedTask(task);
}

void executeCompletedTask(Task* task) {
	printf("***TASK INFO***\n");
	printf("Operation type: %d\n", task->operationType);
	printf("Arguments: %d, %d\n", task->arg1, task->arg2);
	printf("Result: %d\n", task->result);
	printf("\n");
}

void submitTask(Task task) {
    pthread_mutex_lock(&mutexQueue);
    taskQueue[taskCount] = task;
    taskCount++;
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);
}

void* startWorkingThread(void* args) {
    while (1) {
        Task task;

        pthread_mutex_lock(&mutexQueue);
        while (taskCount == 0) {
            pthread_cond_wait(&condQueue, &mutexQueue);
        }

        task = taskQueue[0];
        int i;
        for (i = 0; i < taskCount - 1; i++) {
            taskQueue[i] = taskQueue[i + 1];
        }
        taskCount--;
        pthread_mutex_unlock(&mutexQueue);
        executeTask(&task);
    }
}

void* startReadingThread(void* args) {
    while (1) {
        Task task;

        pthread_mutex_lock(&mutexCompletedQueue);
        while (completedTasksCount == 0) {
           	pthread_cond_wait(&condCompletedQueue, &mutexCompletedQueue);
      	}

        task = completedTasksQueue[0];
        int i;
        for (i = 0; i < completedTasksCount - 1; i++) {
            completedTasksQueue[i] = completedTasksQueue[i + 1];
        }
       	completedTasksCount--;
        pthread_mutex_unlock(&mutexCompletedQueue);
        executeCompletedTask(&task);
    }
}

int main(int argc, char* argv[]) {
    pthread_t workerTh[THREAD_NUM];
    pthread_t readerTh[THREAD_NUM];
    
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_mutex_init(&mutexCompletedQueue, NULL);
    
    pthread_cond_init(&condQueue, NULL);
    pthread_cond_init(&condCompletedQueue, NULL);
    
    for (int i = 0; i < THREAD_NUM; i++) {
        if (pthread_create(&workerTh[i], NULL, &startWorkingThread, NULL) != 0) {
            perror("Failed to create the thread");
        }
        
        if (pthread_create(&readerTh[i], NULL, &startReadingThread, NULL) != 0) {
            perror("Failed to create the thread");
        }

    }
    

    srand(time(NULL));
    for (int i = 0; i < 100; i++) {
        Task t = {
            .taskFunction = i % 2 == 0 ? &sum : &product,
            .arg1 = rand() % 100,
            .arg2 = rand() % 100,
            .operationType = i % 2 == 0 ? SABIRANJE : MNOZENJE
        };
        submitTask(t);
    }

    for (int i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(workerTh[i], NULL) != 0) {
            perror("Failed to join the thread");
        }
        
        if (pthread_join(readerTh[i], NULL) != 0) {
            perror("Failed to join the thread");
        }
    }
    
    pthread_mutex_destroy(&mutexQueue);
    pthread_mutex_destroy(&mutexCompletedQueue);
    
    pthread_cond_destroy(&condQueue);
    pthread_cond_destroy(&condCompletedQueue);
    
    return 0;
}