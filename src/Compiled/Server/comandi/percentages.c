//
// Created by stefano on 21/11/17.
//
#include "Headers.h"
#include <string.h>
#include <netinet/in.h>

int execute(char * args, const backupThread * back, const conf * cfgs, int (*print_f)(const char *, ...)){

    int runningOnly = 0;

    if(strlen(args) > 0){
        if(strcmp(args, "running ") == 0){
            runningOnly = 1;
            goto cycle;
        }

        int index = atoi(args);
        if(index >= cfgs->port_interval) return 1;

        if(back[index].socket == -10){
            print_f("%d. DISCONNESSO\n", index);
            return 0;
        }

        if(back[index].numberOfFiles == 0 || back[index].dimension == 0) return -1;

        float perFile = 1.0f / back[index].numberOfFiles;
        float perc = back[index].transferred / back[index].dimension;
        perc = back[index].filesTransferred * perFile + perc * perFile;

        if(back[index].filesTransferred == back[index].numberOfFiles) perc = 1.0f;

        //struct sockaddr_in * cli = (struct sockaddr_in *) &(back[index].client);

        print_f("\n%d. [%.2f%%]|", index, perc*100);
        for(int i = 0; i < perc*30; i++) print_f("#");
        for(int i = 0; i < 30 - perc*30; i++) print_f("_");
        print_f("| %s\n", back[index].status ? "PAUSED" : "RUNNING");
        return 0;
    }

    cycle:

    for(int index = 0; index < cfgs->port_interval; index++){
        if(back[index].socket == -10){
            if(!runningOnly) print_f("%d. DISCONNESSO\n", index);
            continue;
        }

        if(back[index].numberOfFiles == 0 || back[index].dimension == 0) continue;

        float perFile = 1.0f / back[index].numberOfFiles;
        float perc = back[index].transferred / back[index].dimension;
        perc = back[index].filesTransferred * perFile + perc * perFile;

        if(back[index].filesTransferred == back[index].numberOfFiles) perc = 1.0f;

        //struct sockaddr_in * cli = (struct sockaddr_in *) &(back[index].client);
        print_f("\n%d. [%.2f%%]|", index, perc*100);
        for(int i = 0; i < perc*30; i++) print_f("#");
        for(int i = 0; i < 30 - perc*30; i++) print_f("_");
        print_f("| %s\n", back[index].status ? "PAUSED" : "RUNNING");
    }

    return 0;
}
