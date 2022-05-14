#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <inttypes.h>
#include <strings.h>


int main(int argc, char *argv[]){
    struct tms tmsStart;
    struct tms tmsEnd;
    clock_t start = times(&tmsStart);

    FILE *inFile = fopen(argv[1],"r");
    FILE *outFile = fopen(argv[2],"w");

    char buffer[51];
    int pointer = 0;

    while (fread(buffer+pointer,1,1,inFile)){
        if (buffer[pointer] == '\n'){
            fwrite(buffer,1, pointer+1,outFile);
            pointer = 0;
        }else {
            pointer++;
            if (pointer == 50) {
                fwrite(buffer, 1, pointer, outFile);
                fwrite("\n", 1, 1, outFile);
                pointer = 0;
            }
        }
    }
    if (pointer > 0){
        fwrite(buffer,1,pointer,outFile);
    }

    fclose(inFile);
    fclose(outFile);

    clock_t end = times(&tmsEnd);

    printf("Real Time: %jd, User Time %jd, System Time %jd\n",
           (intmax_t)(end - start),
           (intmax_t)(tmsEnd.tms_utime - tmsStart.tms_utime),
           (intmax_t)(tmsEnd.tms_stime - tmsStart.tms_stime));

    return 0;
}