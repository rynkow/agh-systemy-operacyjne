#include "library.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int maxLineLength = 128;

struct BlockArray* makeBlockArray(int size){
    struct BlockArray *blockArray = calloc(1,sizeof(struct BlockArray));
    blockArray->size = size;
    blockArray->versesBlocks = calloc(size,sizeof(struct VersesBlock*));
    return blockArray;
}

void mergeFiles(struct FilePair *pairs, struct BlockArray *blockArray) {
    FILE *file1 = fopen(pairs->file1,"r");
    FILE *file2 = fopen(pairs->file2,"r");
    FILE *temp = tmpfile();

    while (!feof(file1) || !feof(file2)){
        char line[maxLineLength];
        if (fgets(line, maxLineLength,file1)){
            fputs(line,temp);
            if (feof(file1)){
                fputc('\n',temp);
            }
        }
        if (fgets(line, maxLineLength,file2)){
            fputs(line,temp);
            if (feof(file2)){
                fputc('\n',temp);
            }
        }
    }
    fclose(file1);
    fclose(file2);
    makeVerseBlock(temp,blockArray);
    fclose(temp);
}

int verseCount(struct VersesBlock *verses) {
    return verses->size-verses->noOfRemovedVerses;
}

void removeVerseBlock(struct BlockArray *blockArray, int blockIndex) {
    if (blockIndex >= blockArray->size || blockIndex < 0){
        puts("wrong block index in removeVerseBlock");
        return;
    }
    for (int i = 0; i < blockArray->versesBlocks[blockIndex]->size; ++i) {
        free(blockArray->versesBlocks[blockIndex]->verses[i]);
    }
    free(blockArray->versesBlocks[blockIndex]->verses);
    free(blockArray->versesBlocks[blockIndex]);
    blockArray->versesBlocks[blockIndex] = NULL;
}

void removeVerse(struct BlockArray *blockArray, int blockIndex, int verseIndex) {
    if (blockIndex >= blockArray->size || blockIndex < 0){
        puts("wrong block index in removeVerse");
        return;
    }
    if (verseIndex >= blockArray->versesBlocks[blockIndex]->size || verseIndex < 0){
        puts("wrong verse index in removeVerse");
        return;
    }
    if (blockArray->versesBlocks[blockIndex]->verses[verseIndex] == NULL){
        puts("wrong verse already removed");
        return;
    }
    free(blockArray->versesBlocks[blockIndex]->verses[verseIndex]);
    blockArray->versesBlocks[blockIndex]->verses[verseIndex] = NULL;
    blockArray->versesBlocks[blockIndex]->noOfRemovedVerses += 1;
}

int makeVerseBlock(FILE *tempFile, struct BlockArray *blockArray) {
    struct VersesBlock *versesBlock = malloc(sizeof(struct VersesBlock));
    versesBlock->noOfRemovedVerses=0;
    versesBlock->size = 0;
    char line[maxLineLength];
    rewind(tempFile);
    while (fgets(line, maxLineLength, tempFile)){
        versesBlock->size+=1;
    }
    rewind(tempFile);
    versesBlock->verses = calloc(versesBlock->size, sizeof(char*));

    for (int i = 0; i < versesBlock->size; ++i) {
        versesBlock->verses[i] = calloc(maxLineLength, 1);
    }
    int i = 0;

    while (fgets(versesBlock->verses[i], maxLineLength, tempFile)){
        i += 1;
    }


    for (int j = 0; j < blockArray->size; ++j) {
        if (blockArray->versesBlocks[j] == NULL){
            blockArray->versesBlocks[j] = versesBlock;
            return j;
        }
    }
    puts("BlockArray is full");
    return -1;
}

struct FilePair *stringToFilePair(char *pair) {
    struct FilePair* filePair = malloc(sizeof(struct FilePair));
    char *p = pair;
    int dividerIndex = 0;
    while (*p != ':'){
        dividerIndex += 1;
        if (*p == '\0'){
            return NULL;
        }
        p += 1;
    }
    int endIndex = dividerIndex;
    while (*p != '\0'){
        endIndex += 1;
        p += 1;
    }
    filePair->file1 = calloc(dividerIndex+1, sizeof(char));
    filePair->file2 = calloc(endIndex-dividerIndex, sizeof(char));
    strncpy(filePair->file1,pair,dividerIndex);
    strcpy(filePair->file2,pair+dividerIndex+1);
    return filePair;
}

void printMergedFiles(struct BlockArray *blockArray) {
    for (int i = 0; i < blockArray->size; ++i) {
        if (blockArray->versesBlocks[i] != NULL){
            for (int j = 0; j < blockArray->versesBlocks[i]->size; ++j) {
                if (blockArray->versesBlocks[i]->verses[j] != NULL){
                    puts(blockArray->versesBlocks[i]->verses[j]);
                }
            }
        }
    }
}








