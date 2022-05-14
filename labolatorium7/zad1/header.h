#include <sys/types.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <stdlib.h>

#ifndef header_h
#define header_h

#define OVEN_CAPACITY 5
#define TABLE_CAPACITY 5
#define FREE -1

#define SHARED_MEMORY_FTOK_ID 1
#define OVEN_FREE_SPACE_SEM_FTOK_ID 2
#define OVEN_IN_USE_SEM_FTOK_ID 3
#define TABLE_FREE_SPACE_SEM_FTOK_ID 4
#define TABLE_IN_USE_SEM_FTOK_ID 5

key_t get_shared_memory_key(){
    return ftok(getenv("HOME"), SHARED_MEMORY_FTOK_ID);
}

key_t get_oven_free_space_semaphore_key(){
    return ftok(getenv("HOME"), OVEN_FREE_SPACE_SEM_FTOK_ID);
}

key_t get_table_free_space_semaphore_key(){
    return ftok(getenv("HOME"), TABLE_FREE_SPACE_SEM_FTOK_ID);
}

key_t get_oven_in_use_semaphore_key(){
    return ftok(getenv("HOME"), OVEN_IN_USE_SEM_FTOK_ID);
}

key_t get_table_in_use_semaphore_key(){
    return ftok(getenv("HOME"), TABLE_IN_USE_SEM_FTOK_ID);
}

void decrement_by_one(int semaphoreID){
    struct sembuf sembuf[1];
    sembuf[0].sem_num = 0;
    sembuf[0].sem_flg = 0;
    sembuf[0].sem_op = -1;
    semop(semaphoreID, sembuf, 1);
}

void increment_by_one(int semaphoreID){
    struct sembuf sembuf[1];
    sembuf[0].sem_num = 0;
    sembuf[0].sem_flg = 0;
    sembuf[0].sem_op = 1;
    semop(semaphoreID, sembuf, 1);
}

typedef struct SharedMemory{
    int oven[OVEN_CAPACITY];
    int table[TABLE_CAPACITY];
}SharedMemory;

int pizzas_in_oven(int free_space_in_oven_semaphore){
    return OVEN_CAPACITY - semctl(free_space_in_oven_semaphore, 0, GETVAL);
}

int pizzas_on_table(int free_space_on_table_semaphore){
    return TABLE_CAPACITY - semctl(free_space_on_table_semaphore,0, GETVAL);
}

int put_pizza_in_oven(SharedMemory *sharedMemory, int type){
    for (int i = 0; i < OVEN_CAPACITY; ++i) {
        if (sharedMemory->oven[i] == FREE){
            sharedMemory->oven[i] = type;
            return i;
        }
    }
}

int put_pizza_on_table(SharedMemory *sharedMemory, int type){
    for (int i = 0; i < TABLE_CAPACITY; ++i) {
        if (sharedMemory->table[i] == FREE){
            sharedMemory->table[i] = type;
            return i;
        }
    }
}

#endif
