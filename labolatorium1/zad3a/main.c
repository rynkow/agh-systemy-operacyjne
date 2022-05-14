#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <inttypes.h>



void error(char *message){
    printf("%s\n",message);
    exit(0);
}


int main(int argc, char *argv[]) {

    if (argc < 3)
        error("too little args");

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
            blockArray = makeBlockArray(table_size);
            currentlyProcessedArg += 1;
        }
        else if (strcmp(argv[currentlyProcessedArg],"merge_files") == 0){
            currentlyProcessedArg += 1;
            for (int i = 0; i < size; ++i) {
                struct FilePair *pair = stringToFilePair(argv[currentlyProcessedArg]);
                mergeFiles(pair,blockArray);
                currentlyProcessedArg += 1;
            }
        }
        else if (strcmp(argv[currentlyProcessedArg],"remove_block") == 0){
            currentlyProcessedArg += 1;
            int index = atoi(argv[currentlyProcessedArg]);
            removeVerseBlock(blockArray,index);
            currentlyProcessedArg += 1;
        }
        else if (strcmp(argv[currentlyProcessedArg],"remove_row") == 0){
            currentlyProcessedArg += 1;
            int block_index = atoi(argv[currentlyProcessedArg]);
            currentlyProcessedArg += 1;
            int row_index = atoi(argv[currentlyProcessedArg]);
            removeVerse(blockArray,block_index,row_index);
            currentlyProcessedArg += 1;
        }
        else if (strcmp(argv[currentlyProcessedArg],"print") == 0){
            printMergedFiles(blockArray);
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


    return 0;

}
