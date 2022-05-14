#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include "header.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <time.h>

void printBoard(int b[3][3]){
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (b[i][j] == EMPTY)
                printf(" ");
            else if (b[i][j] == X)
                printf("X");
            else
                printf("O");
            if (j != 2)
                printf("|");
        }
        printf("\n");
    }
}

struct Message message;
int serverSocket;
struct sockaddr *serverAddr;
int mode;
char name[MAX_NAME_LEN];


void handler(int sig){
    message.type = TYPE_DISCONNECTED;
    strcpy(message.name, name);
    sendto(serverSocket, &message, sizeof(message), 0, serverAddr, sizeof(*serverAddr));
    if (mode == MODE_UNIX)
        unlink(name);
    exit(0);
}

void exec_at_exit(){
    if (mode == MODE_UNIX)
        unlink(name);
}

int main(int argc, char **argv) {
    atexit(exec_at_exit);
    signal(SIGINT, handler);
    int serverPort;
    mode = atoi(argv[1]);
    strcpy(name, argv[2]);
    char serverSocketPath[108];

    printf("mode %d\n", mode);
    if (mode == 0) {
        serverPort = atoi(argv[3]);
    } else {
        strcpy(serverSocketPath, argv[3]);
    }


    struct sockaddr_un unixAddr;
    struct sockaddr_in internetAddr;
    struct sockaddr_un clientAddr;
//    struct Message message;
//    int serverSocket;
//    struct sockaddr *serverAddr;

    if (mode == 0) {
        if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }


        internetAddr.sin_family = AF_INET;
        internetAddr.sin_port = htons(serverPort);
        internetAddr.sin_addr.s_addr = INADDR_ANY;


        serverAddr = (struct sockaddr *) &internetAddr;
    } else {
        if ((serverSocket = socket(AF_UNIX, SOCK_DGRAM, 0)) == 0) {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        unixAddr.sun_family = AF_UNIX;
        strcpy(unixAddr.sun_path, serverSocketPath);

        clientAddr.sun_family = AF_UNIX;
        strcpy(clientAddr.sun_path, name);

        serverAddr = (struct sockaddr *) &unixAddr;

        if (bind(serverSocket, (struct sockaddr *)&clientAddr, sizeof(clientAddr))<0)
        {
            perror("unix socket bind failed");
            exit(EXIT_FAILURE);
        }
    }

    strcpy(message.name, name);
    char *buf = malloc(255);
    size_t len = MAX_NAME_LEN;

    message.type = TYPE_NAME;
    strcpy(message.name, name);


    sendto(serverSocket, &message, sizeof(message), 0, serverAddr, sizeof(*serverAddr));


    while (1) {
        recvfrom(serverSocket, &message, sizeof(message), 0, NULL, NULL);
        printf("message received, type: %d\n", message.type);

        if (message.type == TYPE_WAITING_FOR_OPPONENT) {
            printf("waiting for opponent\n");
        } else if (message.type == TYPE_MOVE) {
            if (strcmp(message.name, name) == 0) {
                printf("You start\n");
            } else {
                printf("%s mode a move\n", message.name);
            }
            printBoard(message.board);
            if (message.sign == O)
                printf("Your sign : O\n");
            else
                printf("Your sign : X\n");
            printf("chose a cell:\nverse:\n");
            getline(&buf, &len, stdin);
            buf[1] = '\0';
            int verse = atoi(buf);
            printf("column\n");
            getline(&buf, &len, stdin);
            buf[1] = '\0';
            int column = atoi(buf);

            message.board[verse][column] = message.sign;
            printBoard(message.board);
            strcpy(message.name, name);

            sendto(serverSocket, &message, sizeof(message), 0, serverAddr, sizeof(*serverAddr));
        }
        else if (message.type ==TYPE_NAME_TAKEN){
            printf("name taken\n");
            message.type = TYPE_DISCONNECTED;
            sendto(serverSocket, &message, sizeof(message), 0, serverAddr, sizeof(*serverAddr));
            exit(0);
        }
        else if(message.type == TYPE_WON){
            printf("YOU WON\n");
            message.type = TYPE_DISCONNECTED;
            sendto(serverSocket, &message, sizeof(message), 0, serverAddr, sizeof(*serverAddr));
            exit(0);
        }
        else if(message.type == TYPE_LOST){
            printf("%s made a move\n", message.name);
            printBoard(message.board);
            printf("YOU LOST\n");
            message.type = TYPE_DISCONNECTED;
            sendto(serverSocket, &message, sizeof(message), 0, serverAddr, sizeof(*serverAddr));
            exit(0);
        }
        else if (message.type == TYPE_DRAW){
            if (strcmp(name, message.name) != 0){
                printf("%s made a move\n", message.name);
                printBoard(message.board);
            }
            printf("DRAW\n");
            message.type = TYPE_DISCONNECTED;
            sendto(serverSocket, &message, sizeof(message), 0, serverAddr, sizeof(*serverAddr));
            exit(0);
        }
        else if (message.type == TYPE_DISCONNECTED){
            printf("opponent disconnected\n");
            exit(0);
        }

    }
}



