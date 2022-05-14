#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>


#include <sys/stat.h>
#include <fcntl.h>

#define MAX_COMMAND_NO 3
#define MAX_ARG_NO 3


struct Component{
    char name[50];
    char* command[MAX_COMMAND_NO][MAX_ARG_NO+2];
    int command_no;
};

struct Component strToComponent(char *string){
    struct Component result;
    int pointer = 0;
    while (string[pointer] != ' '){
        result.name[pointer] = string[pointer];
        pointer++;
    }
    result.name[pointer] = '\0';
    pointer+=3;
    for (int i = 0; i < MAX_COMMAND_NO; ++i) {
        for (int j = 0; j < MAX_ARG_NO+1; ++j) {
            int arg_pointer = 0;
            result.command[i][j] = malloc(50);
            char c = string[pointer];
            while (c == ' '){
                pointer++;
                c = string[pointer];
                continue;
            }
            while (c != ' ' && c != '\n'){
                result.command[i][j][arg_pointer] = c;
                arg_pointer++;
                pointer++;
                c = string[pointer];
            }
            result.command[i][j][arg_pointer] = '\0';
            if (c == '\n'){
                result.command[i][j+1] = NULL;
                break;
            }
            else
                pointer++;
            if (string[pointer] == '|'){
                result.command[i][j+1] = NULL;
                pointer++;
                break;
            }
        }
        if (string[pointer] == '\n'){
            result.command_no = i+1;
            break;
        }
    }

    return result;
}

void printComponent( struct Component *component){
    printf("%s =", component->name);
    for (int i = 0; i < component->command_no; ++i) {
        for (int j = 0; j < MAX_ARG_NO; ++j) {
            if (component->command[i][j] == NULL)
                break;
            printf(" %s", component->command[i][j]);
        }
        if (i != component->command_no-1)
            printf(" |");
    }
    printf("\n");
}

void executeComponent(struct Component *component, int inputPipeDescriptor, int outputPipeDescriptor){
    int inMiddlePipeDescriptors[2];
    int outMiddlePipeDescriptors[2];
    printf("executing component: ");
    printComponent(component);
    printf("input: %d, output %d\n", inputPipeDescriptor, outputPipeDescriptor);

    if (component->command_no == 1){
        if (fork() == 0) {
            dup2(inputPipeDescriptor, STDIN_FILENO);
            dup2(outputPipeDescriptor, STDOUT_FILENO);
            execvp(component->command[0][0], component->command[0]);
        }
        return;
    }

    pipe(outMiddlePipeDescriptors);
    printf("created pipe read: %d, write: %d\n", outMiddlePipeDescriptors[0], outMiddlePipeDescriptors[1]);
    if (fork() == 0) {
        dup2(inputPipeDescriptor, STDIN_FILENO);
        dup2(outMiddlePipeDescriptors[1], STDOUT_FILENO);
        close(outMiddlePipeDescriptors[0]);
        execvp(component->command[0][0], component->command[0]);
    } else{
        close(outMiddlePipeDescriptors[1]);
        printf("closed: %d\n", outMiddlePipeDescriptors[1]);
    }
    for (int i = 1; i < component->command_no-1; ++i) {
        inMiddlePipeDescriptors[0] = outMiddlePipeDescriptors[0];
        inMiddlePipeDescriptors[1] = outMiddlePipeDescriptors[1];
        pipe(outMiddlePipeDescriptors);
        printf("created pipe read: %d, write: %d\n", outMiddlePipeDescriptors[0], outMiddlePipeDescriptors[1]);
        if (fork() == 0) {
            dup2(inMiddlePipeDescriptors[0], STDIN_FILENO);
            dup2(outMiddlePipeDescriptors[1], STDOUT_FILENO);
            close(outMiddlePipeDescriptors[0]);
            execvp(component->command[i][0], component->command[i]);
        } else{
            close(inMiddlePipeDescriptors[0]);
            printf("closed: %d\n", inMiddlePipeDescriptors[0]);
            close(outMiddlePipeDescriptors[1]);
            printf("closed: %d\n", outMiddlePipeDescriptors[1]);
        }
    }
    inMiddlePipeDescriptors[0] = outMiddlePipeDescriptors[0];
    inMiddlePipeDescriptors[1] = outMiddlePipeDescriptors[1];
    if (fork() == 0) {
        dup2(inMiddlePipeDescriptors[0], STDIN_FILENO);
        dup2(outputPipeDescriptor, STDOUT_FILENO);
        execvp(component->command[component->command_no-1][0], component->command[component->command_no-1]);
    } else{
        close(inMiddlePipeDescriptors[0]);
        printf("closed: %d\n", inMiddlePipeDescriptors[0]);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2){
        printf("incorrect number of arguments.");
        exit(-1);
    }

    FILE *file = fopen(argv[1], "r");
    int component_no = 0;

    char line[301];
    while (1){
        fgets(line, 300, file);
        if (strchr(line,'=') != NULL){
            component_no++;
        }
        else
            break;
    }

    rewind(file);
    struct Component components[component_no];

    for (int i = 0; i < component_no; ++i) {
        fgets(line, 300, file);
        components[i] = strToComponent(line);
    }

    for (int i = 0; i < component_no; ++i) {
        printComponent(&components[i]);
    }

    fgets(line, 300, file); // pusta linia oddzielająca skladniki od poloecen

    while (fgets(line, 300, file) != NULL){
        printf("processing line %s\n", line);

        char name[50];
        struct Component componentsToProcess[component_no];
        int componentToProcess_no = 0;
        int pointer = 0;
        char c = line[pointer];

        while (c != '\n' && c != '\0'){
            int name_pointer = 0;
            while (c != '|' && c != '\0'  && c != '\n'){
                if (c == ' '){
                    pointer++;
                    c = line[pointer];
                    continue;
                }

                name[name_pointer] = c;
                pointer++;
                name_pointer++;
                c = line[pointer];
            }
            name[name_pointer] = '\0';
            for (int i = 0; i < component_no; ++i) {
                if (strcmp(components[i].name, name) == 0) {
                    componentsToProcess[componentToProcess_no] = components[i];
                    componentToProcess_no++;
                    break;
                }
            }
            if (c == '\0' || c == '\n'){
                break;
            } else{
                pointer++;
                c = line[pointer];
            }
        }

        if (componentToProcess_no == 1) {
            executeComponent(&componentsToProcess[0], STDIN_FILENO, STDOUT_FILENO);
            printf("waiting for children\n");
            while(wait(NULL) > 0);
            printf("finished waiting\n");
        }
        else{
            int inFd[2];
            int outFd[2];

            pipe(outFd);
            executeComponent(&componentsToProcess[0], STDIN_FILENO, outFd[1]);
            close(outFd[1]);

            for (int i = 1; i < componentToProcess_no-1; ++i) {
                inFd[0] = outFd[0];
                inFd[1] = outFd[1];
                pipe(outFd);
                executeComponent(&componentsToProcess[i], inFd[0], outFd[1]);
                close(inFd[0]);
                close(outFd[1]);
            }

            inFd[0] = outFd[0];
            inFd[1] = outFd[1];
            executeComponent(&componentsToProcess[componentToProcess_no-1], inFd[0], STDOUT_FILENO);
            close(inFd[0]);
            printf("waiting for children\n");
            while(wait(NULL) > 0);
            printf("finished waiting\n");

        }
    }

    return 0;
}