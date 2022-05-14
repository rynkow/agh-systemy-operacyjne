#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
int main(){
    sem_t *free_space_on_table_semaphore;
    sem_t *table_in_use_semaphore;

    int sharedMemoryFD;
    SharedMemory *sharedMemory;

    free_space_on_table_semaphore = sem_open(TABLE_FREE_SPACE_SEM_NAME, 0);

    table_in_use_semaphore = sem_open(TABLE_IN_USE_SEM_NAME, 0);

    sharedMemoryFD = shm_open(SHARED_MEMORY_NAME, O_RDWR,0);
    ftruncate(sharedMemoryFD, sizeof(SharedMemory));

    sharedMemory = mmap(NULL, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemoryFD, 0);

    srand(getpid());

    int type;

    while (1){
        if (pizzas_on_table(free_space_on_table_semaphore) == 0)
            continue;

        type = FREE;
        decrement_by_one(table_in_use_semaphore);

        for (int i = 0; i < TABLE_CAPACITY; ++i) {
            if (sharedMemory->table[i] != FREE){
                type = sharedMemory->table[i];
                sharedMemory->table[i] = FREE;
                increment_by_one(free_space_on_table_semaphore);
                printf("%d %ld pobieram pizze %d , ilosc pizz na stole: %d\n", getpid(), time(NULL), type, pizzas_on_table(free_space_on_table_semaphore));
                break;
            }
        }
        increment_by_one(table_in_use_semaphore);

        if (type == FREE){
            continue;
        }
        sleep(4+rand()%2);
        printf("%d %ld dostarczam pizze %d\n", getpid(), time(NULL), type);
        sleep(4+rand()%2);

    }

}
#pragma clang diagnostic pop