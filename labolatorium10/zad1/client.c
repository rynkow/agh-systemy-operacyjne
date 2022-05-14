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


int main(int argc, char **argv) {
    char name[MAX_NAME_LEN];
    char IP[50];
    int serverPort;
    int serverSocket;
    int mode = atoi(argv[1]);
    strcpy(name, argv[2]);
    char serverSocketPath[108];

    printf("mode %d\n", mode);
    if (mode == 0){
        strcpy(IP, argv[3]);
        serverPort = atoi(argv[4]);
    } else{
        strcpy(serverSocketPath, argv[3]);
    }


    if (mode == 0) {
        if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in serverAddress;

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(serverPort);

        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, IP, &serverAddress.sin_addr) <= 0) {
            printf("\nInvalid address/ Address not supported \n");
            return -1;
        }

        if (connect(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
            printf("\nConnection Failed \n");
            return -1;
        }
        printf("conected\n");
    }
    else{
        if ((serverSocket = socket(AF_UNIX, SOCK_STREAM, 0)) == 0) {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_un serverAddress;

        serverAddress.sun_family = AF_UNIX;
        strcpy(serverAddress.sun_path, serverSocketPath);

        if (connect(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
            printf("\nConnection Failed \n");
            return -1;
        }
        printf("conected\n");
    }

    struct Message message;
    strcpy(message.name, name);
    char *buf = malloc(255);
    size_t len = 255;

    message.type = TYPE_NAME;
    strcpy(message.name, name);

    send(serverSocket, &message, sizeof(message), 0);


    while (1){
        read(serverSocket, &message, sizeof(message));

        if (message.type == TYPE_MOVE){
            if (strcmp(message.name, name) == 0){
                printf("You start\n");
            }
            else{
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

            send(serverSocket, &message, sizeof(message), 0);
        }
        else if (message.type == TYPE_WAITING_FOR_OPPONENT){
            printf("waiting for opponent\n");
        }
        else if (message.type == TYPE_WON){
            printf("YOU WON\n");
            exit(0);
        }
        else if(message.type == TYPE_LOST){
            printf("%s mode a move\n", message.name);
            printBoard(message.board);
            printf("YOU LOST\n");
            exit(0);
        }
        else if (message.type == TYPE_DRAW){
            if (strcmp(message.name, name)){
                printf("%s mode a move\n", message.name);
                printBoard(message.board);
            }

            printf("DRAW\n");
            exit(0);
        }
        else if (message.type == TYPE_NAME_TAKEN){
            printf("name taken\n");
            exit(0);
        }
        else if (message.type == TYPE_PING){
            send(serverSocket, &message, sizeof(message), 0);
        }
        else if (message.type == TYPE_DISCONNECTED){
            printf("OPPONENT DISCONNECTED\n");
            exit(0);
        }
    }


}