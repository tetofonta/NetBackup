//
// Created by stefano on 19/11/17.
//

#ifndef NB_TERMINAL_H
#define NB_TERMINAL_H

#include "../Headers.h"

void init_terminal(pid_t serverpid, backupThread *baks, conf *confs);
int serve_ssh_terminal(pid_t serverpid, backupThread *baks, conf *cfgs, sshconf cfg);
void
parseCommand(char *command, size_t len, backupThread *baks, conf *cfgs, int (*printToStream)(const char *format, ...));


#endif //NB_TERMINAL_H
