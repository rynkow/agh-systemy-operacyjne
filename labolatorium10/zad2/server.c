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


struct Client{
    struct sockaddr addr;
    char name[MAX_NAME_LEN];
    struct Client *connectedTo;
    socklen_t addrlen;
};

struct Client clients[MAX_CLIENTS];
struct Client *unpairedClient = NULL;


int playerWon(int board[3][3], int sign){
    int won = 1;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] != sign) won = 0;
        }
        if (won) return 1;
        won = 1;
    }
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[j][i] != sign) won = 0;
        }
        if (won) return 1;
        won = 1;
    }

    for (int i = 0; i < 3; ++i) {
        if (board[i][i] != sign) won = 0;
    }
    if (won) return 1;
    won = 1;
    for (int i = 0; i < 3; ++i) {
        if (board[i][2-i] != sign) won = 0;
    }
    // [0][2] ; [1][1] ; [2][0]
    if (won) return 1;

    return 0;
}

int draw(int board[3][3]){
    for (int i = 0; i <3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == EMPTY)
                return 0;
        }
    }
    return 1;
}

int nameTaken(char *name){
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (strcmp(clients[i].name, name) == 0)
            return 1;
    }
    return 0;
}

struct Client *getClient(char *name){
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (strcmp(clients[i].name, name) == 0)
            return &clients[i];
    }
    return NULL;
}

char serverSocketPath[108];


void handler(int sig){

    unlink(serverSocketPath);
    exit(0);

}


int main(int argc, char **argv) {
    signal(SIGINT, handler);

    srand(time(NULL));

    int serverPortNumber = atoi(argv[1]);

    strcpy(serverSocketPath, argv[2]);
    int internetSocket;
    int unixSocket;
    int pingCounter = 0;


    struct sockaddr_in internetAddress;
    internetAddress.sin_family = AF_INET;
    internetAddress.sin_port = htons(serverPortNumber);
    internetAddress.sin_addr.s_addr = INADDR_ANY;

    struct sockaddr_un unixAddress;
    unixAddress.sun_family = AF_UNIX;
    strcpy(unixAddress.sun_path, serverSocketPath);

    //create a internet socket
    if( (internetSocket = socket(AF_INET , SOCK_DGRAM , 0)) == 0)
    {
        perror("internet socket failed");
        exit(EXIT_FAILURE);
    }

    //create unix socket
    if( (unixSocket = socket(AF_UNIX , SOCK_DGRAM , 0)) == 0)
    {
        perror("unix socket failed");
        exit(EXIT_FAILURE);
    }

    //bind internet socket to port
    if (bind(internetSocket, (struct sockaddr *)&internetAddress, sizeof(internetAddress))<0)
    {
        perror("internet socket bind failed");
        exit(EXIT_FAILURE);
    }

    //bind unix socket
    if (bind(unixSocket, (struct sockaddr *)&unixAddress, sizeof(unixAddress))<0)
    {
        perror("unix socket bind failed");
        exit(EXIT_FAILURE);
    }


    for (int i = 0; i < MAX_CLIENTS; ++i) {
        strcpy(clients[i].name, "");
        clients[i].connectedTo = NULL;
    }

    struct Message message;

    printf("Waiting for connections...\n");

    fd_set fd_read;

    int mode;
    struct sockaddr addr;
    socklen_t  addrlen = sizeof(addr);



    while (1){
        int max_fd = internetSocket;
        FD_ZERO(&fd_read);
        FD_SET(internetSocket, &fd_read);
        FD_SET(unixSocket, &fd_read);
        if (unixSocket > max_fd)
            max_fd = unixSocket;


        // waiting for input
        select( max_fd + 1 , &fd_read , NULL , NULL , NULL);
        printf("input found\n");



        if (FD_ISSET(internetSocket, &fd_read)){
            recvfrom(internetSocket, &message, sizeof(message), 0, &addr, &addrlen);
            mode = MODE_INT;
        }
        else if (FD_ISSET(unixSocket, &fd_read)){
            recvfrom(unixSocket, &message, sizeof(message), 0, &addr, &addrlen);
            mode = MODE_UNIX;
        }
        printf("message from %s, mode: %d, type: %d\n", message.name, mode, message.type);
        printf("sa_family: %d\n", addr.sa_family);
        if (message.type == TYPE_NAME){
            if (nameTaken(message.name)){
                message.type = TYPE_NAME_TAKEN;
                if (mode == MODE_UNIX) {
                    sendto(unixSocket, &message, sizeof(message), 0, &addr, addrlen);
                    printf("sond name taken message | unix\n");
                }
                else {
                    sendto(internetSocket, &message, sizeof(message), 0, &addr, addrlen);
                    printf("sond name taken message | internet\n");
                }
            }
            else{
                struct Client *new_client;
                for (int i = 0; i < MAX_CLIENTS; ++i) {
                    if (strcmp(clients[i].name, "") == 0){
                        new_client = &clients[i];
                        printf("space found at %d\n", i);
                        break;
                    }
                }
                new_client->addr = addr;
                new_client->addrlen = addrlen;
                strcpy(new_client->name, message.name);
                printf("new client %s\n", message.name);

                if (unpairedClient == NULL){
                    unpairedClient = new_client;
                    message.type = TYPE_WAITING_FOR_OPPONENT;
                    if (mode == MODE_UNIX) {
                        int b = sendto(unixSocket, &message, sizeof(message), 0, &new_client->addr, addrlen);
                        printf("send waiting for opponent message | unix , %d\n", b);
                    }
                    else {
                        int b = sendto(internetSocket, &message, sizeof(message), 0, &new_client->addr, addrlen);
                        printf("send waiting for opponent message | int, %d \n", b);

                    }

                    printf("%s waiting for partner\n", unpairedClient->name);
                } else{
                    new_client->connectedTo = unpairedClient;
                    unpairedClient->connectedTo = new_client;
                    unpairedClient = NULL;

                    int start = rand()%2;
                    for (int i = 0; i < 3; ++i)
                        for (int j = 0; j < 3; ++j)
                            message.board[i][j] = EMPTY;

                    message.type = TYPE_MOVE;
                    message.sign = O;

                    if (start){
                        if (addr.sa_family == 1)
                            sendto(unixSocket, &message, sizeof(message), 0, &addr, addrlen);
                        else
                            sendto(internetSocket, &message, sizeof(message), 0, &addr, addrlen);
                    }
                    else{
                        strcpy(message.name, new_client->connectedTo->name);
                        int bytes_send;
                        if (new_client->connectedTo->addr.sa_family == 1) {
                            bytes_send = sendto(unixSocket, &message, sizeof(message), 0, &new_client->connectedTo->addr, new_client->connectedTo->addrlen);
                            printf("sending message trough unix socket\n");
                        }
                        else{
                            bytes_send = sendto(internetSocket, &message, sizeof(message), 0, &new_client->connectedTo->addr, new_client->connectedTo->addrlen);
                            printf("sending message trough unix socket\n");
                        }
                        printf("sent %d bytes to %s player, sock family: %d\n", bytes_send, new_client->connectedTo->name, new_client->connectedTo->addr.sa_family);
                    }
                }
            }
        }
        else if (message.type == TYPE_MOVE){
            struct Client *client = getClient(message.name);
            if (client == NULL)
                continue;
            printf("move message form %s player, sock family: %d\n", client->name, client->addr.sa_family);

            if (playerWon(message.board, message.sign)){
                message.type = TYPE_LOST;
                if (client->connectedTo->addr.sa_family == 1)
                    sendto(unixSocket, &message, sizeof(message), 0, &client->connectedTo->addr, client->connectedTo->addrlen);
                else
                    sendto(internetSocket, &message, sizeof(message), 0, &client->connectedTo->addr, client->connectedTo->addrlen);

                message.type = TYPE_WON;
                if (client->addr.sa_family == 1)
                    sendto(unixSocket, &message, sizeof(message), 0, &client->addr, client->addrlen);
                else
                    sendto(internetSocket, &message, sizeof(message), 0, &client->addr, client->addrlen);

            }
            else if (draw(message.board)){
                message.type = TYPE_DRAW;

                if (client->addr.sa_family == 1)
                    sendto(unixSocket, &message, sizeof(message), 0, &client->connectedTo->addr, client->connectedTo->addrlen);
                else
                    sendto(internetSocket, &message, sizeof(message), 0, &client->connectedTo->addr, client->connectedTo->addrlen);

                if (client->addr.sa_family == 1)
                    sendto(unixSocket, &message, sizeof(message), 0, &client->addr, client->addrlen);
                else
                    sendto(internetSocket, &message, sizeof(message), 0, &client->addr, client->addrlen);
            }
            else {
                message.sign *= -1;
                int bytes_send;
                if (client->connectedTo->addr.sa_family == 1) {
                    bytes_send = sendto(unixSocket, &message, sizeof(message), 0, &client->connectedTo->addr, client->connectedTo->addrlen);
                    printf("sending message through unix socket\n");
                }
                else {
                    bytes_send = sendto(internetSocket, &message, sizeof(message), 0, &client->connectedTo->addr,client->connectedTo->addrlen);
                    printf("sending message through internet socket\n");
                }
                printf("sent %d bytes to %s player, sock family: %d\n", bytes_send, client->connectedTo->name, client->connectedTo->addr.sa_family);
                if (bytes_send == -1)
                    perror("sending move mess error");
            }
        }
        else if(message.type == TYPE_DISCONNECTED){
            struct Client *client;
            client = getClient(message.name);
            if (client != NULL) {
                strcpy(client->name, "");
                if (unpairedClient == client){
                    unpairedClient = NULL;
                }

                if (client->connectedTo != NULL) {
                    message.type = TYPE_DISCONNECTED;

                    if (client->connectedTo->addr.sa_family == 1)
                        sendto(unixSocket, &message, sizeof(message), 0, &client->connectedTo->addr, client->connectedTo->addrlen);
                    else
                        sendto(internetSocket, &message, sizeof(message), 0, &client->connectedTo->addr, client->connectedTo->addrlen);

                    client->connectedTo->connectedTo = NULL;
                    strcpy(client->connectedTo->name, "");
                }
                client->connectedTo = NULL;
            }
        }

    }

    return 1;
}