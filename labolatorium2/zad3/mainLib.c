#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <inttypes.h>
#include <math.h>
#include <strings.h>


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

    FILE *dane = fopen("dane.txt","r");
    FILE *a = fopen("a.txt","w");
    FILE *b = fopen("b.txt","w");
    FILE *c = fopen("c.txt", "w");

    char line[100];
    int noOfEven = 0;
    while (fgets(line,100,dane)){

        int num = atoi(line);
        if (isEven(num))
            noOfEven++;
        if (tensDigit(num) == 7 || tensDigit(num) == 0)
            fputs(line,b);
        if (isSquare(num))
            fputs(line,c);
    }
    fprintf(a,"Liczb parzystych jest %i\n",noOfEven);

    fclose(dane);
    fclose(a);
    fclose(b);
    fclose(c);

    clock_t end = times(&tmsEnd);

    printf("Real Time: %jd, User Time %jd, System Time %jd\n",
           (intmax_t)(end - start),
           (intmax_t)(tmsEnd.tms_utime - tmsStart.tms_utime),
           (intmax_t)(tmsEnd.tms_stime - tmsStart.tms_stime));

    return 0;
}