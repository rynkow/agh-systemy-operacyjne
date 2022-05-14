#include <sys/types.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <stdlib.h>

#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef header_h
#define header_h

#define OVEN_CAPACITY 5
#define TABLE_CAPACITY 5
#define FREE -1

#define SHARED_MEMORY_NAME "sharedMemory"
#define OVEN_FREE_SPACE_SEM_NAME "ovenFreeSpace"
#define OVEN_IN_USE_SEM_NAME "ovenInUse"
#define TABLE_FREE_SPACE_SEM_NAME "tableFreeSpace"
#define TABLE_IN_USE_SEM_NAME "tableInUse"

void decrement_by_one(sem_t *sem){
    sem_wait(sem);
}

void increment_by_one(sem_t *sem){
    sem_post(sem);
}

typedef struct SharedMemory{
    int oven[OVEN_CAPACITY];
    int table[TABLE_CAPACITY];
}SharedMemory;

int pizzas_in_oven(sem_t *free_space_in_oven_semaphore){
    int val;
    sem_getvalue(free_space_in_oven_semaphore, &val);
    return OVEN_CAPACITY - val;
}

int pizzas_on_table(sem_t *free_space_on_table_semaphore){
    int val;
    sem_getvalue(free_space_on_table_semaphore, &val);
    return TABLE_CAPACITY - val;
}

int put_pizza_in_oven(SharedMemory *sharedMemory, int type){
    for (int i = 0; i < OVEN_CAPACITY; ++i) {
        if (sharedMemory->oven[i] == FREE){
            sharedMemory->oven[i] = type;
            return i;
        }
    }
    return -1;
}

int put_pizza_on_table(SharedMemory *sharedMemory, int type){
    for (int i = 0; i < TABLE_CAPACITY; ++i) {
        if (sharedMemory->table[i] == FREE){
            sharedMemory->table[i] = type;
            return i;
        }
    }
    return -1;
}

#endif
