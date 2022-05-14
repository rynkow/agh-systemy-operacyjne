#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <inttypes.h>
#include <strings.h>



void replace(char* inputFileName, char* outputFileName, char* toChange, char* desired){
    FILE *inFile = fopen(inputFileName,"r");
    FILE *outFile = fopen(outputFileName,"w");
    int toChangeLen = 0;
    while (toChange[toChangeLen] != '\0')
        toChangeLen++;
    int desiredLen = 0;
    while (desired[desiredLen] != '\0')
        desiredLen++;

    char buffer[toChangeLen+1];

    buffer[toChangeLen] = '\0';

    char c;

    while (fread(&c,1,1,inFile)){
        if (c != toChange[0]){
            fwrite(&c,1,1,outFile);
        }
        else{
            buffer[0] = c;
            int i = 1;
            for (; i < toChangeLen; ++i) {
                if (!fread(&c,1,1,inFile)){
                    fwrite(buffer,1,i,outFile);
                    break;
                }
                buffer[i] = c;
                if (c != toChange[i]){
                    fseek(inFile,-i,1);
                    fwrite(buffer,1,1,outFile);
                    break;
                }
            }
            if (i == toChangeLen){
                fwrite( desired,1,desiredLen,outFile);
            }
        }
    }

    fclose(inFile);
    fclose(outFile);
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