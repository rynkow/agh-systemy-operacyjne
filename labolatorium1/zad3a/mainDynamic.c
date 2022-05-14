#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <inttypes.h>
#include <dlfcn.h>

void error(char *message){
    printf("%s\n",message);
    exit(0);
}

int main(int argc, char *argv[]) {

    if (argc < 3)
        error("too little args");

    void *lib = dlopen("liblibrary.so", RTLD_LAZY);
    if (!lib)
        error("Dynamic library err");

    struct BlockArray* (*makeBlockArray)(int);
    makeBlockArray = (struct BlockArray* (*)(int))dlsym(lib,"makeBlockArray");
    if (!makeBlockArray)
        error("Dynamic library makeBlockArray err");

    void (*removeVerseBlock)();
    removeVerseBlock = (void (*)())dlsym(lib,"removeVerseBlock");

    void (*removeVerse)();
    removeVerse = (void (*)())dlsym(lib,"removeVerse");

    struct FilePair* (*stringToFilePair)(char*);
    stringToFilePair = (struct FilePair* (*)(char*))dlsym(lib,"stringToFilePair");

    void (*mergeFiles)(struct FilePair*, struct BlockArray*);
    mergeFiles = (void (*)())dlsym(lib,"mergeFiles");

    int size = atoi(argv[1]);

    int currentlyProcessedArg = 2;
    struct BlockArray *blockArray;

    struct tms tmsStart;
    struct tms tmsEnd;
    clock_t start = times(&tmsStart);

    while (currentlyProcessedArg < argc){


        if (strcmp(argv[currentlyProcessedArg],"create_table") == 0){
            currentlyProcessedArg+=1;
            int table_size = atoi(argv[currentlyProcessedArg]);
            blockArray = (*makeBlockArray)(table_size);
            currentlyProcessedArg += 1;
        }
        else if (strcmp(argv[currentlyProcessedArg],"merge_files") == 0){
            currentlyProcessedArg += 1;
            for (int i = 0; i < size; ++i) {
                (*mergeFiles)((*stringToFilePair)(argv[currentlyProcessedArg]),blockArray);
                currentlyProcessedArg += 1;
            }
        }
        else if (strcmp(argv[currentlyProcessedArg],"remove_block") == 0){
            currentlyProcessedArg += 1;
            int index = atoi(argv[currentlyProcessedArg]);
            (*removeVerseBlock)(blockArray,index);
            currentlyProcessedArg += 1;
        }
        else if (strcmp(argv[currentlyProcessedArg],"remove_row") == 0){
            currentlyProcessedArg += 1;
            int block_index = atoi(argv[currentlyProcessedArg]);
            currentlyProcessedArg += 1;
            int row_index = atoi(argv[currentlyProcessedArg]);
            (*removeVerse)(blockArray,block_index,row_index);
            currentlyProcessedArg += 1;
        }
        else{
            printf("%i\n",currentlyProcessedArg);
            error("arguments error - else");
        }


    }

    clock_t end = times(&tmsEnd);

    printf("Real Time: %jd, User Time %jd, System Time %jd\n",
           (intmax_t)(end - start),
           (intmax_t)(tmsEnd.tms_utime - tmsStart.tms_utime),
           (intmax_t)(tmsEnd.tms_stime - tmsStart.tms_stime));

    dlclose(lib);
    return 0;

}
