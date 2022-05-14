#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <inttypes.h>


int main(int argc, char *argv[]){
    struct tms tmsStart;
    struct tms tmsEnd;
    clock_t start = times(&tmsStart);

    FILE *file1, *file2;

    if (argc > 1){
        file1 = fopen(argv[1],"r");
        file2 = fopen(argv[2],"r");
    }
    else{
        int nameMaxLen = 52;
        char name1[nameMaxLen], name2[nameMaxLen];
        puts("Name of the first file:");
        fgets(name1,nameMaxLen,stdin);
        puts("Name of the second file:");
        fgets(name2,nameMaxLen,stdin);

        for (int i = 0; i < nameMaxLen; ++i) {
            if (name1[i] == '\n')
                name1[i] = '\0';
            if (name2[i] == '\n')
                name2[i] = '\0';
        }

        file1 = fopen(name1,"r");
        file2 = fopen(name2,"r");
    }


    while (!feof(file1) || !feof(file2)){
        int c;
        if (!feof(file1)) {
            c = fgetc(file1);
            while (c != '\n' && c > -1) {
                putchar(c);
                c = fgetc(file1);
            }
            putchar('\n');
        }

        if (!feof(file2)){
            c = fgetc(file2);
            while (c != '\n' && c > -1){
                putchar(c);
                c = fgetc(file2);
            }
            putchar('\n');
        }
    }

    fclose(file1);
    fclose(file2);

    clock_t end = times(&tmsEnd);

    printf("Real Time: %jd, User Time %jd, System Time %jd\n",
           (intmax_t)(end - start),
           (intmax_t)(tmsEnd.tms_utime - tmsStart.tms_utime),
           (intmax_t)(tmsEnd.tms_stime - tmsStart.tms_stime));

    return 0;
}