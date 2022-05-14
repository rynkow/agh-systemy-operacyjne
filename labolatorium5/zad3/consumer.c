#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

void putInLine(FILE *file, char *string, int line_no) {
    fseek(file,0,0);
    FILE *tmp = tmpfile();
    char line[301];
    for (int i = 0; i < line_no; ++i) {
        fgets(line, 300, file);
        fputs(line, tmp);
    }
    char c = fgetc(file);

    while (c != '\n') {
        fputc(c, tmp);
        c = fgetc(file);
    }
    fputs(string, tmp);
    fputc(c, tmp);
    c = fgetc(file);
    while (c >= 0) {
        fputc(c, tmp);
        c = fgetc(file);
    }

    fseek(file,0,0);
    fseek(tmp,0,0);

    c = fgetc(tmp);
    while (c >= 0) {
        fputc(c, file);
        c = fgetc(tmp);
    }

}

int main(int argc, char *argv[]) {
    if (argc != 4){
        exit(-1);
    }

    FILE *pipe = fopen(argv[1], "r");
    FILE *output = fopen(argv[2],"w+");
    int N = atoi(argv[3]);

    printf("%ld\n", ftell(output));

    for (int i = 0; i < 20; ++i) {
        fputs("\n",output);
    }

    char c = fgetc(pipe);

    while (c >= 0){
        int verse_no = c - '0';

        char stringToPutInFile[N+1];
        for (int i = 0; i < N; ++i) {
            stringToPutInFile[i] = fgetc(pipe);
        }
        stringToPutInFile[N] = '\0';

        printf("putting %s to verse %d\n", stringToPutInFile, verse_no);
        putInLine(output,stringToPutInFile,verse_no);

        c = fgetc(pipe);
    }


    fclose(output);
    fclose(pipe);

    return 0;
}