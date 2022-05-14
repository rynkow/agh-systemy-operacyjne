
#ifndef commands_h
#define commands_h

#define MAX_CLIENTS  10
#define PROJECT_ID 123
#define MAX_MSG_SIZE 2000

int error(char * msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

typedef enum mtype {
    STOP = 1, DISCONNECT = 2, LIST = 3, CONNECT = 4, INIT = 5, TEXT = 6
} mtype;

typedef struct Message{
    long mtype;
    int sender_id;
    key_t sender_key;
    char m_content[MAX_MSG_SIZE];
} Message;

#endif