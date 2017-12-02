//
// Created by stefano on 25/11/17.
//

#ifndef NB_FS_H
#define NB_FS_H

int is_regular_file(const char *path);
char ** getFiles(char * mdir, int * len, int);

#endif //NB_FS_H
