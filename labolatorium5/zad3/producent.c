#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc != 5){
        printf("incorrect number of arguments.");
        exit(-1);
    }

    FILE *pipe = fopen(argv[1], "w");
    int verse_no = atoi(argv[2]);
    FILE *input = fopen(argv[3],"r");
    int N = atoi(argv[4]);

    char c;

    while (!feof(input)){
        char stringToSend[N+2];
        stringToSend[0] = argv[2][0];

        for (int i = 0; i < N; ++i) {
            c = fgetc(input);
            if (!feof(input)){
                stringToSend[i+1] = c;
            } else{
                stringToSend[i+1] = ' ';
            }
        }
        stringToSend[N+1] = '\0';
        printf("putting to pipe: %s\n", stringToSend);
        fputs(stringToSend, pipe);
        sleep(1);
    }

    fclose(input);
    fclose(pipe);

    printf("cosing producer %d\n", verse_no);

    return 0;
}