#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    pid_t child_pid;
    int n = atoi(argv[1]);
    char *args[]={"./child",NULL};
    for(int i = 0; i < n; i++) {
       child_pid = fork();
       if (child_pid == 0){
           printf("Ten napis zostal wyswietlony przez proces potomny o pid: %d!\n",(int)getpid());
           exit(0);
       }
    }

    return 0;


}