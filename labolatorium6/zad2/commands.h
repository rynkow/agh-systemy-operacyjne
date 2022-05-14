
#ifndef commands_h
#define commands_h

#define MAX_CLIENTS  10
#define MAX_MSG_SIZE 300
#define SERVER_QUEUE_NAME "/SysopServer"

int error(char * msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

typedef enum mtype {
    STOP = 6, DISCONNECT = 5, LIST = 4, CONNECT = 3, INIT = 2, TEXT = 1
} mtype;

typedef struct Message{
    long mtype;
    int sender_id;
    key_t sender_key;
    char m_content[MAX_MSG_SIZE];
} Message;

#endif