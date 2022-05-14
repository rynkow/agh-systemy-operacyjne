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


int clientQueueID = -1;
key_t clientQueueKey = -1;
int clientID = -1;

int serverQueueID = -1;
key_t serverQueueKey = -1;

key_t connectedClientQueueKey = -1;
int connectedClientID = -1;
int connected = 0;

void createClientQueue(){
    char * path = getenv("HOME");
    if (!path)
        error("getenv error");
    clientQueueKey = ftok(path, getpid());
    clientQueueID = msgget(clientQueueKey, IPC_CREAT | IPC_EXCL | 0666);
    if (clientQueueID < 0)
        error("msgget error");

    printf("client queue created");
}

void deleteClientQueue(){
    if (clientQueueID < 0)
        return;

    if (msgctl(clientQueueID, IPC_RMID, NULL) < 0)
        perror("error on deleting server queue\n");
    else
        printf("client queue deleted\n");
}

void init(){
    char * path = getenv("HOME");
    if (!path)
        error("getenv error");
    serverQueueKey = ftok(path, PROJECT_ID);
    serverQueueID = msgget(serverQueueKey, 0);
    if (serverQueueID == -1)
        error("access to server queue error");

    struct Message msg;
    msg.mtype = INIT;
    msg.sender_key = clientQueueKey;

    if (msgsnd(serverQueueID, &msg, MAX_MSG_SIZE, 0) == -1)     error("error while sending INIT message to server");
    if (msgrcv(clientQueueID, &msg, MAX_MSG_SIZE,INIT, 0) == -1)     error("error receiving INIT return message form server");

    clientID = msg.sender_id;
    if (clientID == -1)
        error("server has insufficient space");
    printf("init completed with ID=%d\n", clientID);
}

void disconnect(){
    struct Message msg;
    msg.sender_id = clientID;
    msg.sender_key = clientQueueKey;
    msg.mtype = DISCONNECT;
    if (msgsnd(serverQueueID, &msg, MAX_MSG_SIZE, 0) == -1)     error("error while sending disconnect message to server");
    connected = 0;
    printf("disconnected\n");

}

void processInput(char *input){
    struct Message msg;
    msg.sender_id = clientID;
    msg.sender_key = clientQueueKey;
    if (connected){
        if (strcmp(input, "DISCONNECT") == 0){
            msg.mtype = DISCONNECT;
            if(msgsnd(connectedClientID, &msg, MAX_MSG_SIZE,0) == -1)       error("disconnection message error");
            disconnect();
        }
        else{
            msg.mtype = TEXT;
            strcpy(msg.m_content, input);
            if(msgsnd(connectedClientID, &msg, MAX_MSG_SIZE,0) == -1)       error("text message error");
        }
    }
    else{
        if (strcmp(input, "LIST") == 0){
            msg.mtype = LIST;
            if(msgsnd(serverQueueID, &msg, MAX_MSG_SIZE,0) == -1)       error("msgsnd error");
        }
        else if (strcmp(input, "STOP") == 0){
            msg.mtype = STOP;
            if(msgsnd(serverQueueID, &msg, MAX_MSG_SIZE,0) == -1)       error("msgsnd error");
            exit(0);
        }
        else if (strstr(input, "CONNECT") != NULL){
            msg.mtype = CONNECT;
            strcpy(msg.m_content, input+8);
            if(msgsnd(serverQueueID, &msg, MAX_MSG_SIZE,0) == -1)       error("msgsnd error");
        } else{
            error("unknown command");
        }
    }
}

void processMessage(struct Message *msg){
    if (msg->mtype == LIST){
        printf("AVAILABLE CLIENTS:\n%s\n", msg->m_content);
    }
    else if (msg->mtype == TEXT){
        printf("%d: %s\n", msg->sender_id,msg->m_content);
    }
    else if (msg->mtype == DISCONNECT){
        disconnect();
    }
    else if (msg->mtype == CONNECT){
        if (msg->sender_id == serverQueueID){
            struct Message msgToSend;
            msgToSend.mtype = CONNECT;
            msgToSend.sender_id = clientID;
            msgToSend.sender_key = clientQueueKey;
            connectedClientQueueKey = atoi(msg->m_content);
            connectedClientID = msgget(connectedClientQueueKey, 0);
            connected = 1;
            if (connectedClientID == -1)                                    error("msgget");
            if (msgsnd(connectedClientID, &msgToSend, MAX_MSG_SIZE, 0) == -1)        error("msgsnd");
            printf("connected to %d\n", connectedClientID);
        } else{
            connectedClientQueueKey = msg->sender_key;
            connectedClientID = msgget(connectedClientQueueKey,0);
            if (connectedClientID == -1)        error("msgget");
            connected = 1;
            printf("connected to %d\n", connectedClientID);
        }
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

    struct Message msg;

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

        if (msgrcv(clientQueueID, &msg, MAX_MSG_SIZE, 0, IPC_NOWAIT) != -1)
            processMessage(&msg);
    }

    return 0;
}