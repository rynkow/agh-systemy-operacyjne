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
#include <mqueue.h>

#include "commands.h"

int clientID;
mqd_t clientQueueDescriptor;
char clientQueueName[50];

int connected = 0;
mqd_t connectedClientQueueDescriptor;
char connectedClientQueueName[50];

mqd_t serverQueueDescriptor;

void createClientQueue(){
    struct mq_attr attr;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_maxmsg = 5;

    clientQueueName[0] = '/';
    clientQueueName[1] = '\0';
    char buff[10];
    sprintf(buff,"%d", getpid());
    strcat(clientQueueName, buff);

    clientQueueDescriptor = mq_open(clientQueueName, O_CREAT | O_EXCL | O_NONBLOCK | O_RDONLY,  0666, &attr);
    if (clientQueueDescriptor == -1) error("mq_open");

    printf("client queue created\n");
}

void deleteClientQueue(){
    if (mq_close(serverQueueDescriptor) == -1)
        perror("error on closing client queue\n");
    if (mq_close(clientQueueDescriptor) == -1)
        perror("error on closing client queue\n");
    if (mq_unlink(clientQueueName) == -1)
        perror("error on deleting client queue\n");
    printf("client queue deleted\n");
}

void init(){
    serverQueueDescriptor = mq_open(SERVER_QUEUE_NAME, O_WRONLY | O_NONBLOCK);
    if (serverQueueDescriptor == -1)        error("mq_open");

    if (mq_send(serverQueueDescriptor, clientQueueName, MAX_MSG_SIZE, INIT) == -1)     error("error while sending INIT message to server");
}

void disconnect(){
    char message[MAX_MSG_SIZE];
    sprintf(message, "%d|", clientID);

    if (mq_send(serverQueueDescriptor, message, MAX_MSG_SIZE, DISCONNECT) == -1)     error("error while sending DISCONNECT message to server");
    connected = 0;
    printf("disconnected\n");
}

void processInput(char *input){
    if (connected){
        if (strcmp(input, "DISCONNECT") == 0){
            if (mq_send(connectedClientQueueDescriptor, "", MAX_MSG_SIZE, DISCONNECT) == -1)
                error("error while sending DISCONNECT message to other client");
            disconnect();
        }
        else{
            if (mq_send(connectedClientQueueDescriptor, input, MAX_MSG_SIZE, TEXT) == -1)
                error("error while sending text message to other client");
        }
    }
    else{
        char message[MAX_MSG_SIZE];
        sprintf(message, "%d|", clientID);
        if (strcmp(input, "LIST") == 0){
            if (mq_send(serverQueueDescriptor, message, MAX_MSG_SIZE, LIST) == -1)
                error("error while sending LIST message to server");
        }
        else if (strcmp(input, "STOP") == 0){
            if (mq_send(serverQueueDescriptor, message, MAX_MSG_SIZE, STOP) == -1)
                error("error while sending STOP message to server");
            exit(0);
        }
        else if (strstr(input, "CONNECT") != NULL){
            strcat(message, input+8);

            if (mq_send(serverQueueDescriptor, message, MAX_MSG_SIZE, CONNECT) == -1)
                error("error while sending CONNECT message to server");
        } else{
            error("unknown command");
        }
    }
}

void processMessage(char *message, int priority){
    printf("processing type %d, content: %s\n",priority,message);

    if (priority == INIT){
        int ID = atoi(message);
        if (ID == -1)   error("insufficient space");

        clientID = ID;
        printf("init completed with ID=%d\n",ID);
    }
    else if (priority == LIST){
        printf("AVAILABLE CLIENTS:\n%s\n", message);
    }
    else if (priority == TEXT){
        printf("    %s\n", message);
    }
    else if (priority == DISCONNECT){
        disconnect();
    }
    else if (priority == CONNECT){
        strcpy(connectedClientQueueName, message);
        connectedClientQueueDescriptor = mq_open(message, O_NONBLOCK | O_WRONLY);
        if (connectedClientQueueDescriptor == -1) error("mq_open");
        connected = 1;
    }
}

void handler(int sig_no){
    exit(0);
}

int main(int argc, char *argv[]) {
    atexit(deleteClientQueue);
    signal(SIGINT, handler);
    createClientQueue();
    init();

    fd_set read, write, except;

    FD_ZERO(&except);
    FD_ZERO(&read);
    FD_ZERO(&write);
    FD_SET(0,&read);
    struct timeval time;
    time.tv_sec = 0;
    time.tv_usec = 0;

    char line[MAX_MSG_SIZE+1];

    char msg[MAX_MSG_SIZE];
    unsigned int priority;

    while (1){
        FD_SET(0,&read);
        if (select(1, &read, &write, &except, &time) > 0){
            char c = fgetc(stdin);
            int pointer = 0;
            while (c != '\n') {
                line[pointer] = c;
                pointer++;
                c = fgetc(stdin);
            }
            line[pointer] = '\0';
            processInput(line);
        }

        if (mq_receive(clientQueueDescriptor, msg, MAX_MSG_SIZE, &priority) != -1)
            processMessage(msg, priority);
    }

    return 0;
}