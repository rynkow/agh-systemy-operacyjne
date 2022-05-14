#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include<sys/wait.h>


void handler(int sig, siginfo_t *info, void *pointer){
    printf("handling signal %d send from process %d, si_uid: %d, si_code: %d\n",sig,info->si_pid,info->si_uid, info->si_code);
}

int main(int argc, char *argv[]) {

    struct sigaction act;
    sigset_t mask;

    sigemptyset(&mask);

    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handler;
    act.sa_mask = mask;

    sigaction(SIGUSR1, &act, NULL);
    sigaction(2, &act, NULL);

    raise(SIGUSR1);

    for (int i = 0; i < 10; ++i) {
        sleep(1);
    }

    return 0;
}