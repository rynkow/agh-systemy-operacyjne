#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int lookedForFile(struct dirent *file, char *lookedFor){
    if (file->d_type != DT_REG) {
        return 0;
    }
    int pointer = 0;
    while (file->d_name[pointer] != '.'){
        if (file->d_name[pointer] == '\0'){
            return 0;
        }
        pointer++;
    }
    if (strcmp(file->d_name+pointer, ".txt")!=0){
        return 0;
    }
    return strstr(file->d_name, lookedFor)!=NULL;
}

void searchDir(int depth, DIR *dir, char *textLookedFor, char *path){
    struct dirent *file;

    //printf("searching through %s\n",path);

    while ((file = readdir(dir))) {

        if (strcmp(file->d_name,".")==0 || strcmp(file->d_name,"..")==0)
            continue;

        //printf("processing file %s\n",file->d_name);

        if (file->d_type == DT_DIR && depth > 0){

            char newPath[1000];
            strcpy(newPath,path);

            strcat(newPath,"/");
            strcat(newPath,file->d_name);

            //printf("path to dir %s\n",newPath);
            pid_t pid = fork();

            if (pid == 0) {
                searchDir(depth - 1, opendir(newPath), textLookedFor, newPath);
                exit(0);
            }
        }

        if (lookedForFile(file,textLookedFor)){
            printf("found file: %s, path: %s/%s, by process with pid: %d\n", file->d_name,path,file->d_name,getpid());
        }
    }
    closedir(dir);
}


int main(int argc, char *argv[]) {

    if (argc != 4){
        printf("incorrect no of arguments - should be 2\n");
        exit(0);
    }

    char *startingDirName = argv[1];
    char *textLookedFor = argv[2];
    int depth = atoi(argv[3]);

    DIR *dir = opendir(startingDirName);

    searchDir(depth,dir, textLookedFor, startingDirName);

    return 0;
}