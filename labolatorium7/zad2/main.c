#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <time.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "header.h"


int cooks;
int deliverers;
pid_t *cooks_pid;
pid_t *deliverers_pid;
sem_t *free_space_in_oven_semaphore;
sem_t *free_space_on_table_semaphore;
sem_t *oven_in_use_semaphore;
sem_t *table_in_use_semaphore;

int sharedMemoryFD;
SharedMemory *sharedMemory;

void handler(){
    for (int i = 0; i < cooks; ++i) {
        kill(cooks_pid[i], SIGKILL);
    }
    for (int i = 0; i < deliverers; ++i) {
        kill(deliverers_pid[i], SIGKILL);
    }

    sem_unlink(TABLE_FREE_SPACE_SEM_NAME);
    sem_unlink(TABLE_IN_USE_SEM_NAME);
    sem_unlink(OVEN_IN_USE_SEM_NAME);
    sem_unlink(OVEN_FREE_SPACE_SEM_NAME);

    shm_unlink(SHARED_MEMORY_NAME);

    free(deliverers_pid);
    free(cooks_pid);
    exit(0);
}


int main(int argc, char *argv[]) {
    if (argc != 3){
        printf("incorrect number of arguments");
        exit(1);
    }
    cooks = atoi(argv[1]);
    deliverers = atoi(argv[2]);
    cooks_pid = malloc(sizeof(pid_t)* cooks);
    deliverers_pid = malloc(sizeof(pid_t)* deliverers);


    free_space_in_oven_semaphore = sem_open(OVEN_FREE_SPACE_SEM_NAME, O_CREAT | O_EXCL, 0666, 5);

    free_space_on_table_semaphore = sem_open(TABLE_FREE_SPACE_SEM_NAME, O_CREAT | O_EXCL, 0666, 5);

    oven_in_use_semaphore = sem_open(OVEN_IN_USE_SEM_NAME, O_CREAT | O_EXCL, 0666, 1);
    table_in_use_semaphore = sem_open(TABLE_IN_USE_SEM_NAME, O_CREAT | O_EXCL, 0666, 1);

    sharedMemoryFD = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_EXCL | O_RDWR, 0666 );
    ftruncate(sharedMemoryFD, sizeof(SharedMemory));

    sharedMemory = mmap(NULL, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemoryFD, 0);
    for (int i = 0; i < OVEN_CAPACITY; ++i) {
        sharedMemory->oven[i] = FREE;
    }
    for (int i = 0; i < TABLE_CAPACITY; ++i) {
        sharedMemory->table[i] = FREE;
    }

    signal(SIGINT, handler);

    for (int i = 0; i < cooks; ++i) {
        if ((cooks_pid[i] = fork()) == 0){
            execlp("./cook", "cook", NULL);
        }
    }
    for (int i = 0; i < deliverers; ++i) {
        if ((deliverers_pid[i] = fork()) == 0){
            execlp("./deliverer", "supplier", NULL);
        }
    }

    wait(NULL);
}
