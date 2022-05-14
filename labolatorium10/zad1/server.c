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
    int socket;
    char name[MAX_NAME_LEN];
    struct Client *connectedTo;
    int pinged;
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



void disconnect(int i){
    printf("client %s disconnected\n", clients[i].name);
    if (clients[i].connectedTo != NULL){
        struct Message message;
        message.type = TYPE_DISCONNECTED;
        if (clients[i].pinged){
            send(clients[i].socket, &message, sizeof(message), 0);
        }
        send(clients[i].connectedTo->socket, &message, sizeof(message), 0);
    }
    clients[i].connectedTo = NULL;
    strcpy(clients[i].name, "");
    clients[i].pinged = 0;
    if (unpairedClient == &clients[i])
        unpairedClient = NULL;
    close(clients[i].socket);
    clients[i].socket = 0;
}

char serverSocketPath[108];

void handler(int sig){
    struct Message message;
    message.type = TYPE_DISCONNECTED;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].socket > 0){
            send(clients[i].socket, &message, sizeof(message), 0);
        }
    }
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
    if( (internetSocket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
    {
        perror("internet socket failed");
        exit(EXIT_FAILURE);
    }

    //create unix socket
    //create a internet socket
    if( (unixSocket = socket(AF_UNIX , SOCK_STREAM , 0)) == 0)
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

    //listen on internet socket, max 10 connections
    if (listen(internetSocket, 10) < 0)
    {
        perror("internet socket listen");
        exit(EXIT_FAILURE);
    }

    //listen on unix socket, max 10 connections
    if (listen(unixSocket, 10) < 0)
    {
        perror("unix socket listen");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clients[i].socket = 0;
        clients[i].connectedTo = NULL;
        clients[i].pinged = 0;
    }
    struct Message message;

    printf("Waiting for connections...\n");
    size_t internetAddrLen = sizeof(internetAddress);
    size_t unixAddrLen = sizeof(unixAddress);
    fd_set fd_read;

    while (1){
        int max_fd = internetSocket;
        FD_ZERO(&fd_read);
        FD_SET(internetSocket, &fd_read);
        FD_SET(unixSocket, &fd_read);
        if (unixSocket > max_fd)
            max_fd = unixSocket;

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (clients[i].socket > 0){
                FD_SET(clients[i].socket, &fd_read);
                if (clients[i].socket > max_fd)
                    max_fd = clients[i].socket;
            }
        }

        // waiting for input
        select( max_fd + 1 , &fd_read , NULL , NULL , NULL);
        printf("input found\n");

        if (FD_ISSET(internetSocket, &fd_read)){
            int new_socket;
            if ((new_socket = accept(internetSocket, (struct sockaddr *)&internetAddress, (socklen_t*)&internetAddrLen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("New connection, socket fd is %d\n" , new_socket);

            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if (clients[i].socket == 0){
                    clients[i].socket = new_socket;
                    break;
                }
            }
        }

        if (FD_ISSET(unixSocket, &fd_read)){
            int new_socket;
            if ((new_socket = accept(unixSocket, (struct sockaddr *)&unixAddress, (socklen_t*)&unixAddrLen))<0)
            {
                perror("unix accept");
                exit(EXIT_FAILURE);
            }

            printf("New connection, socket fd is %d\n" , new_socket);

            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if (clients[i].socket == 0){
                    clients[i].socket = new_socket;
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (clients[i].socket > 0 && FD_ISSET(clients[i].socket, &fd_read)){
                int client_socket = clients[i].socket;

                //Check if it was for closing , and also read the
                //incoming message
                if ((read( client_socket , &message, sizeof(message))) == 0)
                {
                    //Somebody disconnected
                    //Close the socket and mark as 0 in list for reuse
//                    printf("client %d disconnected\n", client_socket);
//                    close(client_socket);
//                    clients[i].socket = 0;
//                    clients[i].connectedTo = NULL;
//                    strcpy(clients[i].name, "");
                    disconnect(i);
                }

                    //Message incoming
                else
                {
                    if (message.type == TYPE_NAME) {
                        int nameTaken = 0;
                        for (int j = 0; j < MAX_NAME_LEN; ++j) {
                            if (strcmp(clients[j].name, message.name) == 0)
                                nameTaken = 1;
                            break;
                        }
                        if (nameTaken) {
                            message.type = TYPE_NAME_TAKEN;
                            send(client_socket, &message, sizeof(message), 0);
                            printf("%s name taken\n", message.name);
                        } else {
                            strcpy(clients[i].name, message.name);
                            printf("client %d set to %s\n", client_socket, message.name);

                            //pair if able
                            if (unpairedClient == NULL){
                                message.type = TYPE_WAITING_FOR_OPPONENT;
                                unpairedClient = &clients[i];
                                send(client_socket, &message, sizeof(message), 0);
                            }
                            else{
                                int start = rand()%2;
                                unpairedClient->connectedTo = &clients[i];
                                clients[i].connectedTo = unpairedClient;
                                unpairedClient = NULL;
                                message.type = TYPE_MOVE;
                                for (int j = 0; j < 3; ++j) {
                                    for (int k = 0; k < 3; ++k) {
                                        message.board[j][k] = EMPTY;
                                    }
                                }

                                message.sign = O;

//                                printf("sending fintrst move to %s\n", clients[i].name);
//                                send(client_socket, &message, sizeof(message), 0);

                                if (start){
                                    printf("wylosowano gracza %s\n", clients[i].name);
                                    send(client_socket, &message, sizeof(message), 0);
                                }
                                else{
                                    strcpy(message.name, clients[i].connectedTo->name);
                                    printf("wylosowano gracza %s\n", clients[i].connectedTo->name);
                                    send(clients[i].connectedTo->socket, &message, sizeof(message), 0);
                                }
                            }

                        }
                    }
                    else if (message.type == TYPE_MOVE){
                        printf("move message form %s\n", message.name);
                        int opponent = clients[i].connectedTo->socket;
                        if (playerWon(message.board, message.sign)){
                            message.type = TYPE_WON;
                            send(client_socket, &message, sizeof(message), 0);
                            message.type = TYPE_LOST;
                            send(opponent, &message, sizeof(message), 0);
                        }
                        else if (draw(message.board)){
                            message.type = TYPE_DRAW;
                            send(client_socket, &message, sizeof(message), 0);
                            send(opponent, &message, sizeof(message), 0);
                        }
                        else {
                            message.sign *= -1;

                            printf("sending move form %s to %s\n", message.name, clients[i].connectedTo->name);
                            send(opponent, &message, sizeof(message), 0);
                        }
                    }
                    else if (message.type == TYPE_PING){
                        clients[i].pinged = 0;
                    }
                }
            }
        }

        //ping
        pingCounter++;
        if (pingCounter> MAX_CLIENTS*3){
            pingCounter = 0;
            for (int i = 0; i < MAX_CLIENTS; ++i) {
                message.type = TYPE_PING;
                if (clients[i].socket > 0){
                    if (clients[i].pinged){
                        disconnect(i);
                        continue;
                    }
                    send(clients[i].socket, &message, sizeof(message), 0);
                    clients[i].pinged = 1;
                    printf("pinging client %d\n", clients[i].socket);
                }
            }
        }

    }


}