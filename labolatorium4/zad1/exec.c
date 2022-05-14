#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>


void handler(int sig){
    printf("signal no %i handled by handler in exec\n", sig);
}

int main(int argc, char *argv[]) {
    char *arg = argv[1];

    if (strcmp(arg,"pending") == 0){
        sigset_t pending_mask;
        sigpending(&pending_mask);

        sigpending(&pending_mask);

        if (sigismember(&pending_mask,SIGUSR1))
            printf("signal SIGUSR1 is pending in exec\n");
        else
            printf("signal SIGUSR1 is NOT pending in exec\n");

    }
    else {
        printf("rising SIGUSR1 in exec\n");
        raise(SIGUSR1);
        return 0;
    }


    return 0;
}