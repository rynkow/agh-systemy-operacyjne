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

#define FREE 0
#define CONNECTED 1

struct Client{
    int ID;
    char queueName[50];
    mqd_t queueDescriptor;
    int state;
};

mqd_t serverQueueDescriptor = -1;
struct Client clients[MAX_CLIENTS];

void createServerQueue(){
    struct mq_attr attr;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_maxmsg = 10;
    attr.mq_flags = 0;
    attr.mq_curmsgs = 0;
    serverQueueDescriptor = mq_open(SERVER_QUEUE_NAME, O_RDONLY | O_CREAT | O_NONBLOCK,  0666, &attr);
    if (serverQueueDescriptor == -1) error("mq_open creat");

    printf("server queue created\n");
}

void deleteServerQueue(){
    if (mq_close(serverQueueDescriptor) == -1)
        perror("error on closing server queue\n");
    if (mq_unlink(SERVER_QUEUE_NAME) == -1)
        perror("error on deleting server queue\n");
    printf("server queue deleted\n");
}

void handler(int sig_no){
    exit(0);
}

void initClient( char *clientQueueName){
    struct Client newClient;
    strcpy(newClient.queueName, clientQueueName);
    newClient.queueDescriptor = mq_open(clientQueueName, O_NONBLOCK | O_WRONLY);
    newClient.ID = -1;
    newClient.state = FREE;

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].ID == -1){
            newClient.ID = i;
            clients[i] = newClient;
            break;
        }
    }

    char id[4];
    sprintf(id, "%d", newClient.ID);

    if (mq_send(newClient.queueDescriptor, id, 4, INIT) == -1) error("mq_send");
    if (newClient.ID == -1)
        if (mq_close(newClient.queueDescriptor) == -1)  error("mq_close");

}

void removeClient(int clientID){
    if (clients[clientID].ID == -1)
        error("removing nonexistent client error");
    if (mq_close(clients[clientID].queueDescriptor) == -1) error("mq_close");
    clients[clientID].ID = -1;
}

int getSenderId(char * message){
    char id[5];
    int pointer = 0;
    while (message[pointer] != '|'){
        id[pointer] = message[pointer];
        pointer++;
    }
    id[pointer] = '\0';
    return atoi(id);
}

void getMessageContent(char *content,char *message){
    int pointer = 0;
    while (message[pointer] != '|')
        pointer++;
    pointer++;
    strcpy(content, message+pointer);
}

void processMessage(char *message, int priority){
    printf("processing type %d, content: %s\n",priority,message);
    if (priority == INIT){
        initClient(message);
        return;
    }

    int senderID = getSenderId(message);
    char content[MAX_MSG_SIZE];
    getMessageContent(content, message);

    if (priority == STOP){
        removeClient(senderID);
    }
    else if (priority == LIST){
        char list[MAX_MSG_SIZE];
        list[0] = '\0';
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (clients[i].ID != -1 && clients[i].state == FREE){
                char buff[100];
                sprintf(buff, "%d\n",clients[i].ID);
                strcat(list, buff);
            }
        }

        if (mq_send(clients[senderID].queueDescriptor, list, MAX_MSG_SIZE, LIST) == -1)     error("mq_send LIST");
    }
    else if (priority == CONNECT){
        int requestedID = atoi(content);
        if (clients[requestedID].ID == -1 || clients[requestedID].state == CONNECTED) error("invalid connect");
        if (mq_send(clients[senderID].queueDescriptor, clients[requestedID].queueName, MAX_MSG_SIZE, CONNECT) )
            error("msgsnd");
        if (mq_send(clients[requestedID].queueDescriptor, clients[senderID].queueName, MAX_MSG_SIZE, CONNECT) )
            error("msgsnd");
        clients[requestedID].state = CONNECTED;
        clients[senderID].state = CONNECTED;
    }
    else if (priority == DISCONNECT){
        clients[senderID].state = FREE;
    }
    else error("invalid priority");

}

int main(int argc, char *argv[]) {
    atexit(deleteServerQueue);
    signal(SIGINT, handler);
    createServerQueue();
    char msg[MAX_MSG_SIZE];
    unsigned int priority;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clients[i].ID = -1;
    }

    while (1){
        if (mq_receive(serverQueueDescriptor, msg, MAX_MSG_SIZE, &priority) != -1)
            processMessage(msg, priority);
    }
    return 0;
}