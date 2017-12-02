//
// Created by stefano on 19/11/17.
//

#ifndef NB_COMMANDS_H
#define NB_COMMANDS_H

void parseCmd(char * cmd, char * buffer, int socket, sockaddr * client, socklen_t len, conf * cfgs, backupThread * processi);

#endif //NB_COMMANDS_H
