#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/ipc.h>


int main(int argc, char *argv[]) {

////   pid_t pid = fork();
////    while (1){
////        if (pid == 0){
////            char *line;
////            line = calloc(200, 1);
////            int maxSize = 199;
////            getline(&line, &maxSize, stdin);
////            printf("%s\n", line);
////        }else{
////            printf("incoming text\n");
////            sleep(3);
////        }
////    }
//
//    fd_set read, write, except;
//
//    FD_ZERO(&except);
//    FD_ZERO(&read);
//    FD_ZERO(&write);
//    FD_SET(0,&read);
//    struct timeval time;
//    time.tv_sec = 0;
//    time.tv_usec = 0;
//
//    while (1){
//        FD_SET(0,&read);
//        int i =select(1, &read, &write, &except, &time);
//        if (i > 0){
//            char line[200];
////            fgets(line, 199, stdin);
////
////            printf("%s\n", line);
//            char c = fgetc(stdin);
//            while (c != '\n') {
//                printf("'%c', %d\n", c, c);
//                c = fgetc(stdin);
//            }
//
//        }
//        else{
////            printf("...\n");
////            sleep(2);
//        }
//    }

    char line[40] = "CONNECT 10101";
    char res[40];
    strcpy(res, line+8);
    printf("%s\n",res);

    return 0;
}