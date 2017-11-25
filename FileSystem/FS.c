//
// Created by stefano on 25/11/17.
//
#include "../Compatibility/macros.h"
#include "FS.h"
#include <malloc.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>

int is_regular_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}
char stringa[2048];

char ** getFiles(char * mdir, int * len, int depth){
    DIR *dir;
    *len = 0;
    char ** out;
    out = (char **) malloc(1*sizeof(char *));
    struct dirent *ent;
    if ((dir = opendir (mdir)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if(strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
                strcpy(stringa, mdir);
                strcat(stringa, ent->d_name);
                if(is_regular_file(stringa)) {
                    (*len)++;
                    out = (char **) realloc(out, *len * sizeof(char *));
                    out[*len - 1] = (char *) malloc(strlen(ent->d_name) + strlen(mdir));
                    strcpy(out[*len - 1], mdir);
                    strcpy(out[*len - 1] + (strlen(mdir)), ent->d_name);
                } else {
//                    strcat(stringa, "/");
//                    if(depth > 0){
//                        printf("%d %s\n", depth, stringa);
//                        char ** rout;
//                        int rlen;
//                        rout = getFiles(stringa, &rlen, depth-1);
//                        //(*len) += rlen;
//                        //out = (char **) realloc(out, *len * sizeof(char *));
//                        //memcpy(out + ((*len) - rlen), rout, rlen*sizeof(char *));
//                    }
                }
            }
        }
        closedir(dir);
    }
    return out;
}