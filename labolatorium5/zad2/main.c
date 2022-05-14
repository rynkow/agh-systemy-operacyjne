#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc == 4){
        char *address = argv[1];
        char *title = argv[2];
        char *content = argv[3];
//        Wersja bez popen
//        int fd[2];
//        pipe(fd);
//        if (fork() == 0){
//            dup2(fd[1],STDOUT_FILENO);
//            close(fd[0]);
//            execlp("echo", "echo", content, NULL);
//        }
//        close(fd[1]);
//        if (fork()==0){
//            dup2(fd[0],STDIN_FILENO);
//            execlp("mail", "mail", "-s", title, address, NULL );
//        }
//        close(fd[0]);

        char command[200];
        command[0] = '\0';
        strcat(command,"mail -s'");
        strcat(command, title);
        strcat(command, "' ");
        strcat(command,address);
        FILE *mail = popen(command,"w");
        fputs(content,mail);
        pclose(mail);
        return 0;
    }

    if (strcmp("data", argv[1]) == 0){
        FILE *sort = popen("sort -k3 -k 1M -k2 -k4", "w");
        FILE *mailList = popen("cat mail.txt", "r");

        char line[101];

        while (fgets(line, 100, mailList) != NULL){
            fputs(line,sort);
        }

        pclose(mailList);
        pclose(sort);

    }

    if (strcmp("nadawca", argv[1]) == 0){
        FILE *sort = popen("sort -k5", "w");
        FILE *mailList = popen("cat mail.txt", "r");

        char line[101];

        while (fgets(line, 100, mailList) != NULL){
            fputs(line,sort);
        }

        pclose(mailList);
        pclose(sort);
    }

    return 0;
}