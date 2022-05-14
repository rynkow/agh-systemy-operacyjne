#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/times.h>
#include <inttypes.h>
#include <math.h>



int isSquare(int n){
    double m = fmod(sqrt(n),1);
    if (m < 0.0000000000001 || m > 0.99999999999999)
        return 1;
    return 0;
}

int isEven(int n){
    return n%2 == 0;
}

int tensDigit(int n){
    return (n%100)/10;
}



int main(int argc, char *argv[]){
    struct tms tmsStart;
    struct tms tmsEnd;
    clock_t start = times(&tmsStart);

    int dane = open("dane.txt",O_RDONLY);
    int a = open("a.txt",O_WRONLY);
    int b = open("b.txt",O_WRONLY);
    int c = open("c.txt", O_WRONLY);
    ftruncate(a,0);
    ftruncate(b,0);
    ftruncate(c,0);


    char line[100];
    int noOfEven = 0;
    int pointer = 0;


    while (read(dane,line+pointer,1)){
        pointer++;
        if (line[pointer-1] == '\n'){
            line[pointer] = '\0';
            int num = atoi(line);
            if (isEven(num))
                noOfEven++;
            if (tensDigit(num) == 7 || tensDigit(num) == 0)
                write(b,line,pointer);
            if (isSquare(num))
                write(c,line,pointer);
            pointer = 0;
        }
    }
    if (pointer > 0){
        line[pointer] = '\0';
        int num = atoi(line);
        if (isEven(num))
            noOfEven++;
        if (tensDigit(num) == 7 || tensDigit(num) == 0)
            write(b,line,pointer);
        if (isSquare(num))
            write(c,line,pointer);
    }

    write(a,"Liczb parzystych jest ",22);

    char evens[20];
    sprintf(evens,"%i\n",noOfEven);

    pointer = 0;
    while (evens[pointer]!='\0') {
        write(a, evens + pointer, 1);
        pointer++;
    }

    close(dane);
    close(a);
    close(b);
    close(c);

    clock_t end = times(&tmsEnd);

    printf("Real Time: %jd, User Time %jd, System Time %jd\n",
           (intmax_t)(end - start),
           (intmax_t)(tmsEnd.tms_utime - tmsStart.tms_utime),
           (intmax_t)(tmsEnd.tms_stime - tmsStart.tms_stime));

    return 0;
}