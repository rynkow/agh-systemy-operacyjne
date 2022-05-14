#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include<sys/wait.h>


void handler(int sig){
    printf("handling signal %d \n",sig);
    for (int i = 0; i < 10; ++i) {
        printf("%d\n",i);
        sleep(1);
    }
}

int main(int argc, char *argv[]) {

    struct sigaction act;
    sigset_t mask;

    sigemptyset(&mask);

    act.sa_flags = SA_NODEFER;
    act.sa_handler = handler;
    act.sa_mask = mask;

    sigaction(SIGUSR1, &act, NULL);
    sigaction(2, &act, NULL);

    for (int i = 0; i < 10; ++i) {
        sleep(1);
    }

    return 0;
}