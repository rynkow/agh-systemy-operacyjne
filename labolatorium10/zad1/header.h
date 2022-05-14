
#ifndef HEADERFILE_H
#define HEADERFILE_H

#define MAX_NAME_LEN 50
#define MAX_CLIENTS 10

#define TYPE_NAME 1
#define TYPE_MOVE 2
#define TYPE_LOST 3
#define TYPE_WON 4
#define TYPE_WAITING_FOR_OPPONENT 5
#define TYPE_DISCONNECTED 6
#define TYPE_NAME_TAKEN 7
#define TYPE_DRAW 8
#define TYPE_PING 9

#define X -1
#define O 1
#define EMPTY 0

struct Message{
    int sign;
    int type;
    int board[3][3];
    char name[MAX_NAME_LEN];
} Message;

#endif