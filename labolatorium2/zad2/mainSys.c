#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/times.h>
#include <inttypes.h>

int main(int argc, char *argv[]){
    struct tms tmsStart;
    struct tms tmsEnd;
    clock_t start = times(&tmsStart);

    char *fileName = argv[2];
    char wanted = argv[1][0];

    int file = open(fileName, O_RDONLY);

    char line[256];
    int pointer = 0;
    int validLine = 0;

    while (read(file, line+pointer,1)){
        if (line[pointer] == wanted)
            validLine = 1;
        if (line[pointer] == '\n'){
            if (validLine){
                for (int i = 0; i <= pointer; ++i) {
                    putchar(line[i]);
                }
            }
            validLine = 0;
            pointer = 0;
        }
        else
            pointer++;
    }

    close(file);

    clock_t end = times(&tmsEnd);

    printf("Real Time: %jd, User Time %jd, System Time %jd\n",
           (intmax_t)(end - start),
           (intmax_t)(tmsEnd.tms_utime - tmsStart.tms_utime),
           (intmax_t)(tmsEnd.tms_stime - tmsStart.tms_stime));

    return 0;
}