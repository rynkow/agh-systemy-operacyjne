#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/times.h>
#include <inttypes.h>



void replace(char* inputFileName, char* outputFileName, char* toChange, char* desired){
    int inFile = open(inputFileName,O_RDONLY);
    int outFile = open(outputFileName,O_WRONLY);
    ftruncate(outFile,0);
    int toChangeLen = 0;
    while (toChange[toChangeLen] != '\0')
        toChangeLen++;
    int desiredLen = 0;
    while (desired[desiredLen] != '\0')
        desiredLen++;

    char buffer[toChangeLen+1];

    buffer[toChangeLen] = '\0';

    char c;

    while (read(inFile,&c,1)){
        if (c != toChange[0]){
            write(outFile,&c,1);
        }
        else{
            buffer[0] = c;
            int i = 1;
            for (; i < toChangeLen; ++i) {
                if (!read(inFile,&c,1)){
                    write(outFile,buffer,i);
                    break;
                }
                buffer[i] = c;
                if (c != toChange[i]){
                    lseek(inFile,-i,SEEK_CUR);
                    write(outFile,buffer,1);
                    break;
                }
            }
            if (i == toChangeLen){
                write(outFile, desired,desiredLen);
            }
        }
    }

    close(inFile);
    close(outFile);
    return;
}


int main(int argc, char *argv[]){
    struct tms tmsStart;
    struct tms tmsEnd;
    clock_t start = times(&tmsStart);

    replace("../../latin.txt","out.txt","lex","HALO!!!");

    clock_t end = times(&tmsEnd);

    printf("Real Time: %jd, User Time %jd, System Time %jd\n",
           (intmax_t)(end - start),
           (intmax_t)(tmsEnd.tms_utime - tmsStart.tms_utime),
           (intmax_t)(tmsEnd.tms_stime - tmsStart.tms_stime));

    return 0;
}