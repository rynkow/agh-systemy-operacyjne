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

#include "commands.h"

#define FREE 0
#define CONNECTED 1

int serverQueueID = -1;
int serverQueueKey;
int clientIDs[MAX_CLIENTS] = {-1};
int clientKeys[MAX_CLIENTS];
int clientStatus[MAX_CLIENTS];

void createServerQueue(){
    char * path = getenv("HOME");
    if (!path)
        error("getenv error");
    serverQueueKey = ftok(path, PROJECT_ID);
    serverQueueID = msgget(serverQueueKey, IPC_CREAT | IPC_EXCL | 0666);
    if (serverQueueID < 0)
        error("msgget error");

    printf("server queue created\n");
}

void deleteServerQueue(){
    if (serverQueueID < 0)
        return;

    if (msgctl(serverQueueID, IPC_RMID, NULL) < 0)
        perror("error on deleting server queue\n");
    else
        printf("server queue deleted\n");
}

void handler(int sig_no){
    exit(0);
}

int initClient(key_t clientKey){
    int clientID = msgget(clientKey, 0);
    if (clientID == -1)
        error("msgget error");

    int foundFreeSpace = 0;

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clientIDs[i] == -1){
            clientIDs[i] = clientID;
            clientKeys[i] = clientKey;
            clientStatus[i] = FREE;
            foundFreeSpace = 1;
            break;
        }
    }

    struct Message msg;
    msg.mtype = INIT;
    if (foundFreeSpace)
        msg.sender_id = clientID;
    else
        msg.sender_id = -1;

    if (msgsnd(clientID, &msg, MAX_MSG_SIZE, 0) == -1) error("msgsnd");

    return -1;
}

void removeClient(int clientID){
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clientIDs[i] == clientID){
            clientIDs[i] = -1;
            return;
        }
    }

    error("removing nonexistent client error");
}

void processMessage(struct Message *msg){
    printf("processing type: %ld\n", msg->mtype);
    char list[MAX_MSG_SIZE];
    int requestedID;
    int requestedKey = -1;
    struct Message returnMsg;
    returnMsg.sender_key = serverQueueKey;
    returnMsg.sender_id = serverQueueID;

    switch (msg->mtype) {
        case STOP:
            removeClient(msg->sender_id);
            break;
        case DISCONNECT:
            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if (clientIDs[i] == msg->sender_id){
                    clientStatus[i] = FREE;
                    break;
                }
            }
            break;
        case LIST:
            list[0] = '\0';
            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if (clientIDs[i] != -1 && clientStatus[i] == FREE){
                    char buff[100];
                    sprintf(buff, "%d\n",clientIDs[i]);
                    strcat(list, buff);
                }
            }
            returnMsg.mtype = LIST;
            strcpy(returnMsg.m_content, list);
            if (msgsnd(msg->sender_id, &returnMsg, MAX_MSG_SIZE, 0) == -1) error("msgsnd");
            break;
        case INIT:
            initClient(msg->sender_key);
            break;
        case CONNECT:
            requestedID = atoi(msg->m_content);
            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if (clientIDs[i] == requestedID){
                    clientStatus[i] = CONNECT;
                    requestedKey = clientKeys[i];
                }
                if (clientIDs[i] == msg->sender_id){
                    clientStatus[i] = CONNECT;
                }
            }
            if (requestedKey == -1) error("invalid connect");
            sprintf(returnMsg.m_content,"%d",requestedKey);
            returnMsg.mtype = CONNECT;
            if (msgsnd(msg->sender_id, &returnMsg, MAX_MSG_SIZE, 0) == -1) error("msgsnd");
            break;
        default:
            break;
    }
}

int main(int argc, char *argv[]) {
    atexit(deleteServerQueue);
    signal(SIGINT, handler);
    createServerQueue();
    struct Message msg;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clientIDs[i] = -1;
    }

    while (1){
        if (msgrcv(serverQueueID, &msg, MAX_MSG_SIZE, STOP, IPC_NOWAIT) != -1)
            processMessage(&msg);
        else if (msgrcv(serverQueueID, &msg, MAX_MSG_SIZE, DISCONNECT, IPC_NOWAIT) != -1)
            processMessage(&msg);
        else if (msgrcv(serverQueueID, &msg, MAX_MSG_SIZE, LIST, IPC_NOWAIT) != -1)
            processMessage(&msg);
        else if (msgrcv(serverQueueID, &msg, MAX_MSG_SIZE, 0, IPC_NOWAIT) != -1)
            processMessage(&msg);
    }
    return 0;
}