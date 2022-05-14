#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int counter;
int signal_no;
sigset_t mask;

void handler1(int sig){
    counter+=1;
    sigsuspend(&mask);
}

void handler2(int sig){
    printf("sender received %d signals back, should be %d",counter,signal_no);
    exit(0);
}

int main(int argc, char *argv[]) {
    pid_t catcher_pid = atoi(argv[1]);
    signal_no = atoi(argv[2]);
    char *sending_mode = argv[3];

    counter = 0;

    sigfillset(&mask);
    sigdelset(&mask,SIGUSR1);
    sigdelset(&mask,SIGUSR2);
    sigdelset(&mask,SIGRTMIN);
    sigdelset(&mask,SIGRTMIN+1);

    sigprocmask(SIG_BLOCK, &mask, NULL);

    signal(SIGUSR1,handler1);
    signal(SIGRTMIN,handler1);
    signal(SIGUSR2,handler2);
    signal(SIGRTMIN+1,handler2);

    if (strcmp(sending_mode,"KILL") == 0){
        for (int i = 0; i < signal_no; ++i) {
            kill(catcher_pid,SIGUSR1);
            printf("sending %d signal to catcher\n",i);
        }
        printf("sending ending signal to catcher\n");
        kill(catcher_pid,SIGUSR2);
    }
    else if (strcmp(sending_mode,"SIGQUEUE") == 0){
        union sigval val;
        val.sival_int = 0;
        for (int i = 0; i < signal_no; ++i) {
            sigqueue(catcher_pid, SIGUSR1, val);
            printf("sending %d signal to catcher\n",i);
        }
        printf("sending ending signal to catcher\n");
        sigqueue(catcher_pid, SIGUSR2, val);
    }
    else if (strcmp(sending_mode,"SIGRT") == 0){
        for (int i = 0; i < signal_no; ++i) {
            kill(catcher_pid, SIGRTMIN);
            printf("sending %d signal to catcher\n",i);
        }
        printf("sending ending signal to catcher\n");
        kill(catcher_pid, SIGRTMIN+1);
    }
    printf("waiting for catcher response\n");
    sigsuspend(&mask);

    return 0;
}