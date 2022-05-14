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
    sem_t *free_space_in_oven_semaphore;
    sem_t *free_space_on_table_semaphore;
    sem_t *oven_in_use_semaphore;
    sem_t *table_in_use_semaphore;

    int sharedMemoryFD;
    SharedMemory *sharedMemory;

    int type;
    int occupied_oven_spot;

    free_space_in_oven_semaphore = sem_open(OVEN_FREE_SPACE_SEM_NAME, 0);

    free_space_on_table_semaphore = sem_open(TABLE_FREE_SPACE_SEM_NAME, 0);

    oven_in_use_semaphore = sem_open(OVEN_IN_USE_SEM_NAME, 0);
    table_in_use_semaphore = sem_open(TABLE_IN_USE_SEM_NAME, 0);

    sharedMemoryFD = shm_open(SHARED_MEMORY_NAME, O_RDWR,0);
    ftruncate(sharedMemoryFD, sizeof(SharedMemory));

    sharedMemory = mmap(NULL, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemoryFD, 0);

    srand(getpid());

    while (1){
        type = rand()%10;
        printf("%d %ld przygotowuje pizze %d\n", getpid(), time(NULL), type);
        sleep(1 + rand()%2);

        decrement_by_one(free_space_in_oven_semaphore);
        decrement_by_one(oven_in_use_semaphore);

        occupied_oven_spot = put_pizza_in_oven(sharedMemory, type);

        increment_by_one(oven_in_use_semaphore);
        printf("%d %ld dodał pizze %d na miejscu %d, liczba pizz w piecu: %d\n", getpid(), time(NULL), type, occupied_oven_spot, pizzas_in_oven(free_space_in_oven_semaphore));

        sleep(4+rand()%2);

        decrement_by_one(oven_in_use_semaphore);

        sharedMemory->oven[occupied_oven_spot] = FREE;

        increment_by_one(free_space_in_oven_semaphore);
        printf("%d %ld wyjął pizze %d z pieca, ilość pizz w piecu: %d\n", getpid(), time(NULL), type, pizzas_in_oven(free_space_in_oven_semaphore));

        increment_by_one(oven_in_use_semaphore);

        decrement_by_one(free_space_on_table_semaphore);
        decrement_by_one(table_in_use_semaphore);

        put_pizza_on_table(sharedMemory, type);
        printf("%d %ld położył pizze %d na stole, ilosc pizz na stole: %d\n", getpid(), time(NULL), type, pizzas_on_table(free_space_on_table_semaphore));

        increment_by_one(table_in_use_semaphore);

    }

}
#pragma clang diagnostic pop