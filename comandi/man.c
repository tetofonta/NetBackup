//
// Created by stefano on 21/11/17.
//
#include "Headers.h"
#include <string.h>
#include <stdint.h>
#include <malloc.h>
#include <dirent.h>

void getFiles(char * mdir, int (*print_f)(const char *, ...)){
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (mdir)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            print_f("%s\n", ent->d_name);
        }
        closedir(dir);
    }
}

char comando[1024];
int execute(char * args, backupThread * back, conf * cfgs, int (*print_f)(const char *, ...)){

    if(strlen(args) == 0){
        print_f("Usage man [command name]\nOUTPUT: Manual for the specified command\n\n");
        return 0;
    }
    if(strlen(args) > 2) args[strlen(args)-1] = '\0';
    sprintf(comando, "./comandi/%s.txt", args);
    FILE * instruction = fopen(comando, "r");
    if(instruction == NULL){
        printf("Cannot find required documentation.\n%s\n", comando);
		getFiles(".", printf);
        return -1;
    }

    fseek(instruction, 0, SEEK_END);
    size_t toprint = ftell(instruction);
    rewind(instruction);

    char * documentation = malloc((toprint + 1) * sizeof(char));
    documentation[toprint] = '\0';

    fread(documentation, sizeof(char), toprint, instruction);
    print_f("%s", documentation);
	fclose(instruction);
    free(documentation);

    return 0;
}
