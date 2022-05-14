#include <bits/types/FILE.h>

#ifndef ZAD1_LIBRARY_H
#define ZAD1_LIBRARY_H

int maxLineLength;

struct FilePair{
    char* file1;
    char* file2;
};

struct VersesBlock{
    int size;
    int noOfRemovedVerses;
    char **verses;
};

struct BlockArray{
    int size;
    struct VersesBlock **versesBlocks;
};

struct BlockArray* makeBlockArray(int size);

void mergeFiles(struct FilePair *pair, struct BlockArray *blockArray);

int makeVerseBlock(FILE *tempFile, struct BlockArray *blockArray);

int verseCount(struct VersesBlock *verses);

void removeVerse(struct BlockArray *blockArray, int blockIndex, int verseIndex);

void removeVerseBlock(struct BlockArray *blockArray, int blockIndex);

void printMergedFiles(struct BlockArray *blockArray);

struct FilePair* stringToFilePair(char* pair);

#endif //ZAD1_LIBRARY_H
