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

    int inFile = open(argv[1],O_RDONLY);
    int outFile = open(argv[2],O_WRONLY);
    ftruncate(outFile,0);

    char buffer[51];
    int pointer = 0;

    while (read(inFile,buffer+pointer,1)){
        if (buffer[pointer] == '\n'){
            write(outFile, buffer, pointer+1);
            pointer = 0;
        } else {
            pointer++;
            if (pointer == 50) {
                write(outFile, buffer, pointer);
                write(outFile, "\n", 1);
                pointer = 0;
            }
        }
    }
    if (pointer > 0){
        write(outFile,buffer,pointer);
    }

    close(inFile);
    close(outFile);

    clock_t end = times(&tmsEnd);

    printf("Real Time: %jd, User Time %jd, System Time %jd\n",
           (intmax_t)(end - start),
           (intmax_t)(tmsEnd.tms_utime - tmsStart.tms_utime),
           (intmax_t)(tmsEnd.tms_stime - tmsStart.tms_stime));

    return 0;
}