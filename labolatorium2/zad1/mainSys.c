#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/times.h>
#include <inttypes.h>

int eof(int file){
    char c;
    int bytesread = read(file,&c,1);
    if (bytesread)
        lseek(file,-1,SEEK_CUR);
    return !bytesread;
}

int main(int argc, char *argv[]){
    struct tms tmsStart;
    struct tms tmsEnd;
    clock_t start = times(&tmsStart);

    int file1, file2;

    if (argc > 1){
        file1 = open(argv[1],O_RDONLY);
        file2 = open(argv[2],O_RDONLY);
    }
    else{
        int nameMaxLen = 52;
        int pointer = 0;
        char buff;
        char name1[nameMaxLen], name2[nameMaxLen];

        write(1,"Name of the first file:\n",25);
        read(0,&buff,1);
        while (buff!='\n'){
            name1[pointer] = buff;
            read(0,&buff,1);
            pointer++;
        }
        name1[pointer] = '\0';

        write(1,"Name of the second file:\n",26);
        pointer = 0;
        read(0,&buff,1);
        while (buff!='\n'){
            name2[pointer] = buff;
            read(0,&buff,1);
            pointer++;
        }
        name2[pointer] = '\0';


        file1 = open(name1,O_RDONLY);
        file2 = open(name2,O_RDONLY);
    }


    while (!eof(file1) || !eof(file2)){
        char c;
        if (!eof(file1)) {
            while (read(file1,&c,1)) {
                if (c == '\n')
                    break;
                write(1,&c,1);
            }
            write(1,"\n",1);
        }

        if (!eof(file2)) {
            while (read(file2,&c,1)) {
                if (c == '\n')
                    break;
                write(1,&c,1);
            }
            write(1,"\n",1);
        }
    }

    close(file1);
    close(file2);

    clock_t end = times(&tmsEnd);

    printf("Real Time: %jd, User Time %jd, System Time %jd\n",
           (intmax_t)(end - start),
           (intmax_t)(tmsEnd.tms_utime - tmsStart.tms_utime),
           (intmax_t)(tmsEnd.tms_stime - tmsStart.tms_stime));

    return 0;
}