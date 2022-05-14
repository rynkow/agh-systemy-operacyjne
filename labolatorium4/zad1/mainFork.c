#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>


void handler(int sig){
    printf("signal no %i handled by handler in process with pid %d\n", sig, getpid());
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
    printf("rising SIGUSR1 in process %d\n",getpid());
    raise(SIGUSR1);

    if (strcmp(arg,"pending") == 0){
        sigset_t pending_mask;
        sigpending(&pending_mask);

        if (sigismember(&pending_mask,SIGUSR1))
            printf("signal SIGUSR1 is pending in process with pid: %d\n",getpid());
        else
            printf("signal SIGUSR1 is NOT pending in process with pid: %d\n",getpid());

        pid_t pid;
        pid = fork();

        if (pid == 0){
            sigpending(&pending_mask);

            if (sigismember(&pending_mask,SIGUSR1))
                printf("signal SIGUSR1 is pending in process with pid: %d\n",getpid());
            else
                printf("signal SIGUSR1 is NOT pending in process with pid: %d\n",getpid());

            return 0;
        }
    }
    else {
        pid_t pid;
        pid = fork();

        if (pid == 0){
            printf("rising SIGUSR1 in process %d\n",getpid());
            raise(SIGUSR1);
            return 0;
        }
    }

    return 0;
}