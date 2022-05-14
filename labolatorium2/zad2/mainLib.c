#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <inttypes.h>

int charInLine(char wanted, char *line){
    char *p = line;
    while (*p != '\0'){
        if (*p == wanted)
            return 1;
        p++;
    }
    return 0;
}

int main(int argc, char *argv[]){
    struct tms tmsStart;
    struct tms tmsEnd;
    clock_t start = times(&tmsStart);

    char *fileName = argv[2];
    char wanted = argv[1][0];

    FILE* file = fopen(fileName,"r");

    char line[256];

    while (fgets(line,256,file)){
        if (charInLine(wanted, line))
            printf("%s",line);
    }
    fclose(file);

    clock_t end = times(&tmsEnd);

    printf("Real Time: %jd, User Time %jd, System Time %jd\n",
           (intmax_t)(end - start),
           (intmax_t)(tmsEnd.tms_utime - tmsStart.tms_utime),
           (intmax_t)(tmsEnd.tms_stime - tmsStart.tms_stime));

    return 0;
}