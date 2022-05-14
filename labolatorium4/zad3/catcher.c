#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int counter;
char *sending_mode;
sigset_t mask;
pid_t sender_pid;

void handler1(int sig){
    counter+=1;
    sigsuspend(&mask);
}

void handler2(int sig, siginfo_t *info, void *pointer){
    sender_pid = info->si_pid;
    printf("catcher received %d signals, sending back to %d.\n",counter,sender_pid);
    if (strcmp(sending_mode,"KILL") == 0){
        for (int i = 0; i < counter; ++i) {
            kill(sender_pid,SIGUSR1);
            printf("sending %d signal to sender\n",i);
        }
        printf("sending ending signal to sender\n");
        kill(sender_pid,SIGUSR2);
    }
    else if (strcmp(sending_mode,"SIGQUEUE") == 0){
        union sigval val;
        val.sival_int = 0;
        for (int i = 0; i < counter; ++i) {
            sigqueue(sender_pid, SIGUSR1, val);
            printf("sending %d signal to sender\n",i);
        }
        printf("sending ending signal to sender\n");
        sigqueue(sender_pid, SIGUSR2, val);
    }
    else if (strcmp(sending_mode,"SIGRT") == 0){
        for (int i = 0; i < counter; ++i) {
            kill(sender_pid, SIGRTMIN);
            printf("sending %d signal to sender\n",i);
        }
        printf("sending ending signal to sender\n");
        kill(sender_pid, SIGRTMIN+1);
    }
    exit(0);
}

int main(int argc, char *argv[]) {
    printf("Catcher PID: %d\n", getpid());
    counter = 0;

    if (argc < 2)
        sending_mode = "KILL";
    else
        sending_mode = argv[1];

    sigfillset(&mask);
    sigdelset(&mask,SIGUSR1);
    sigdelset(&mask,SIGUSR2);
    sigdelset(&mask,SIGRTMIN);
    sigdelset(&mask,SIGRTMIN+1);


    sigprocmask(SIG_BLOCK, &mask, NULL);

    signal(SIGUSR1,handler1);
    signal(SIGRTMIN,handler1);

    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handler2;
    sigaction(SIGUSR2, &act, NULL);
    sigaction(SIGRTMIN+1, &act, NULL);

    sigsuspend(&mask);

    return 0;
}