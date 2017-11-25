//
// Created by stefano on 21/11/17.
//
#include "Headers.h"
#include <string.h>

int execute(char * args, backupThread * back, conf * cfgs, int (*print_f)(const char *, ...)){

    if(strlen(args) > 0){
        int index = atoi(args);
        if(index >= cfgs->port_interval) return 1;

        if(back[index].socket == -10){
            print_f("%d. DISCONNESSO\n", index);
        }

        if(back[index].numberOfFiles == 0 || back[index].dimension == 0) return -1;

        float perFile = 1.0f / back[index].numberOfFiles;
        float perc = back[index].transferred / back[index].dimension;
        perc = back[index].filesTransferred * perFile + perc * perFile;

        if(back[index].filesTransferred == back[index].numberOfFiles) perc = 1.0f;

        print_f("\n[%.2f%%]|", perc*100);
        for(int i = 0; i < perc*30; i++) print_f("#");
        for(int i = 0; i < 30 - perc*30; i++) print_f("_");
        print_f("|\n");
    }

    return 0;
}