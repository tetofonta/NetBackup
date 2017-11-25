//
// Created by stefano on 19/11/17.
//
#include <stdio.h>
#include <cstring>
#include "Headers.h"
#include "commands.h"

static void cmd_NMR(int server, conf * configs, sockaddr * client, socklen_t len){
    sendto(server, &(configs->port_interval), sizeof(int), 0, client, len);
}

static void cmd_GET(int server, conf * configs, sockaddr * client, socklen_t len, backupThread * processi){
    for (int i = 0; i < configs->port_interval; i++)
        sendto(server, &processi[i], sizeof(backupThread), 0, client, len);
}

static void cmd_STP(char * buffer, backupThread * processi, conf * cfgs){
    int id;
    char foo;
    sscanf(buffer, "%c%c%c%d", &foo, &foo, &foo, &id);
    printf("Stopping server %d\n", id);
    if(id < 0 || id >= cfgs->port_interval) return;
    if(processi[id].socket > 0) shutdown(processi[id].socket, SHUT_RDWR);
    else printf("Backup already terminated\n");
}

static void cmd_TOG(char * buffer, backupThread * processi, conf * cfgs){
    int id;
    char foo;
    sscanf(buffer, "%c%c%c%d", &foo, &foo, &foo, &id);
    if(id < 0 || id >= cfgs->port_interval) return;
    if(processi[id].status == 0) printf("Pausing server %d\n", id);
    else printf("Restarting server %d\n", id);
    processi[id].status = !processi[id].status;
}

static void cmd_SET(int server, conf * configs, sockaddr * client, socklen_t len){
    sendto(server, &configs, sizeof(configs), 0, client, len);
}

void parseCmd(char * cmd, char * buffer, int socket, sockaddr * client, socklen_t len, conf * cfgs, backupThread * processi){
    if(!strcmp(cmd, "NMR"))
        cmd_NMR(socket, cfgs, client, len);

    else if(!strcmp(cmd, "GET"))
        cmd_GET(socket, cfgs, client, len, processi);

    else if (!strcmp(cmd, "STP"))
        cmd_STP(buffer, processi, cfgs);

    else if (!strcmp(cmd, "TOG"))
        cmd_TOG(buffer, processi, cfgs);

    else if (!strcmp(cmd, "SET"))
        cmd_SET(socket, cfgs, client, len);

}