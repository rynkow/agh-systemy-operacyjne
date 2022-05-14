#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>


void handler(int sig){
    printf("signal no %i handled by handler in parent process with pid %d\n", sig, getpid());
}

int main(int argc, char *argv[]) {
    if (argc != 2){
        printf("incorrect argument count\n");
        return 1;
    }
    char *arg = argv[1];
    if (strcmp(arg,"ignore") == 0){
        signal(SIGUSR1,SIG_IGN);
    }
    else if (strcmp(arg,"handler") == 0){
        signal(SIGUSR1,handler);
    }
    else if (strcmp(arg,"mask") == 0){
        sigset_t newmask;
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        sigprocmask(SIG_BLOCK,&newmask,NULL);
    }
    else if (strcmp(arg,"pending") == 0){
        sigset_t newmask;
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        sigprocmask(SIG_BLOCK,&newmask,NULL);
    }
    else {
        printf("incorrect argument\n");
        return 1;
    }
    printf("rising SIGUSR1 in parent process %d\n",getpid());
    raise(SIGUSR1);

    execvp("./exec",argv);

    return 0;
}