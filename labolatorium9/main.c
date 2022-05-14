#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>


int elfsWaiting = 0;
int reindeersWaiting = 0;
int noOfDeliveredPresents;

sem_t presentsDelivered;
sem_t problemsSolved;
sem_t elfWaitingSpace;

pthread_mutex_t waiting_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t shouldSantaWake = PTHREAD_COND_INITIALIZER;

pthread_t santa_thread;
pthread_t elf_threads[10];
pthread_t reindeer_threads[9];
pid_t main_pid;

int helpedElfsIDs[3];

void *santa(void *ptr){
    //czeka na 3 elfy lub 9 reniferow

    while (1) {
        pthread_mutex_lock(&waiting_mutex);
        while (reindeersWaiting < 9 && elfsWaiting < 3) {
            pthread_cond_wait(&shouldSantaWake, &waiting_mutex);
        }
        printf("Mikołaj: budzę się\n");
        if (reindeersWaiting == 9) {
            printf("Mikołaj: dostarczam zabawki\n");
            //rozwozi przez 2-4s
            sleep(2 + rand() % 3);
            for (int i = 0; i < 9; ++i) {
                sem_post(&presentsDelivered);
            }
            noOfDeliveredPresents++;
            reindeersWaiting = 0;
        }
        if (elfsWaiting == 3) {
            printf("Komunikat: Mikołaj: rozwiązuje problemy elfów %d %d %d ID\n", helpedElfsIDs[0], helpedElfsIDs[1], helpedElfsIDs[2]);
            //pomaga
            sleep(1 + rand() % 2);
            for (int i = 0; i < 3; ++i) {
                sem_post(&problemsSolved);
            }
            elfsWaiting = 0;
        }
        pthread_mutex_unlock(&waiting_mutex);

        if (noOfDeliveredPresents == 4){
            //zakoncz
            for (int i = 0; i < 9; ++i) {
                pthread_kill(reindeer_threads[i], SIGKILL);
            }
            for (int i = 0; i < 10; ++i) {
                pthread_kill(elf_threads[i], SIGKILL);
            }
            kill(main_pid, SIGKILL);
            exit(0);
        }
    }

    return ptr;
}

void *reindeer(void *ptr){
    int reindeerID = *((int*)ptr);

    while (1){
        //wakacje przez 5-10s
        sleep(4 + rand() % 2);

        //wraca
        pthread_mutex_lock(&waiting_mutex);
        reindeersWaiting++;
        printf("Renifer: czeka %d reniferów na Mikołaja, %d\n", reindeersWaiting, reindeerID);
        if (reindeersWaiting == 9){
            printf("Renifer: wybudzam Mikołaja, %d\n", reindeerID);
            pthread_cond_broadcast(&shouldSantaWake);
        }
        pthread_mutex_unlock(&waiting_mutex);


        //dostarcza
        sem_wait(&presentsDelivered);

        //wakacje...
    }

    return ptr;
}

void *elf(void *ptr){
    int elfID = *((int*)ptr);

    while (1){
        //Pacuje przez losowy okres czasu 2-5s
        sleep(4 + rand() % 2);

        //czeka na miejsce
        if (elfsWaiting == 3){
            printf("Elf: czeka na powrót elfów, %d\n", elfID);
        }
        sem_wait(&elfWaitingSpace);
        //zgłasza problem
        pthread_mutex_lock(&waiting_mutex);
        helpedElfsIDs[elfsWaiting] = elfID;
        elfsWaiting++;
        printf("Elf: czeka %d elfów na Mikołaja, %d\n", elfsWaiting, elfID);
        if (elfsWaiting == 3){
            printf("Elf: wybudzam Mikołaja, %d\n", elfID);
            pthread_cond_broadcast(&shouldSantaWake);
        }
        pthread_mutex_unlock(&waiting_mutex);
        //czeka na rozwiazanie

        sem_wait(&problemsSolved);
        //wraca
        sem_post(&elfWaitingSpace);

    }

    return ptr;
}

int main(int argc, char **argv){
    sem_init(&problemsSolved, 0, 0);
    sem_init(&presentsDelivered, 0, 0);
    sem_init(&elfWaitingSpace, 0, 3);

    main_pid = getpid();

    pthread_create(&santa_thread, NULL, &santa , NULL);

    for (int i = 0; i < 9; ++i) {
        int *ptr = malloc(sizeof(int));
        *ptr = i;
        pthread_create(reindeer_threads+i, NULL, &reindeer, ptr);
    }
    for (int i = 0; i < 10; ++i) {
        int *ptr = malloc(sizeof(int));
        *ptr = i;
        pthread_create(elf_threads+i, NULL, &elf, ptr);
    }
    pthread_join(santa_thread, NULL);



    return 0;
}