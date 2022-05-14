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

#include "header.h"


int cooks;
int deliverers;
pid_t *cooks_pid;
pid_t *deliverers_pid;
int free_space_in_oven_semaphore;
int free_space_on_table_semaphore;
int oven_in_use_semaphore;
int table_in_use_semaphore;

int sharedMemoryID;
SharedMemory *sharedMemory;

void handler(){
    for (int i = 0; i < cooks; ++i) {
        kill(cooks_pid[i], SIGKILL);
    }
    for (int i = 0; i < deliverers; ++i) {
        kill(deliverers_pid[i], SIGKILL);
    }

    semctl(free_space_in_oven_semaphore,0,IPC_RMID);
    semctl(free_space_on_table_semaphore,0,IPC_RMID);
    semctl(table_in_use_semaphore,0,IPC_RMID);
    semctl(oven_in_use_semaphore,0,IPC_RMID);

    shmctl(sharedMemoryID, IPC_RMID, NULL);

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


    struct sembuf sembuf[1];

    free_space_in_oven_semaphore = semget(get_oven_free_space_semaphore_key(), 1, 0666 | IPC_CREAT | IPC_EXCL);
    sembuf[0].sem_num=0;
    sembuf[0].sem_flg = 0;
    sembuf[0].sem_op = OVEN_CAPACITY;
    semop(free_space_in_oven_semaphore, sembuf, 1);

    free_space_on_table_semaphore = semget(get_table_free_space_semaphore_key(), 1, 0666 | IPC_CREAT | IPC_EXCL);
    sembuf[0].sem_op = TABLE_CAPACITY;
    semop(free_space_on_table_semaphore, sembuf, 1);

    sembuf->sem_op = 1;
    oven_in_use_semaphore = semget(get_oven_in_use_semaphore_key(), 1, 0666 | IPC_CREAT | IPC_EXCL);
    semop(oven_in_use_semaphore, sembuf, 1);
    table_in_use_semaphore = semget(get_table_in_use_semaphore_key(), 1, 0666 | IPC_CREAT | IPC_EXCL);
    semop(table_in_use_semaphore, sembuf, 1);

    sharedMemoryID = shmget(get_shared_memory_key(), sizeof (SharedMemory), 0666 | IPC_CREAT | IPC_EXCL);
    sharedMemory = shmat(sharedMemoryID, NULL, 0);
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
