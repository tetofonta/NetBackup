/**
 * \file FS.c
 * \version 1.0
 * \author Stefano
 * \date 25-11-2017
 * \brief Operazioni sul FileSystem
 */
#include <malloc.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include "../Compatibility/macros.h"
#include "FS.h"

/**
 * \brief Determina se un file esiste o è una cartella
 * \bug File Inesistente o cartella inesistente causa segfault
 * @param path[in] File da verificare
 * @return
 */
int is_regular_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

/**
 * \brief Ottiene i file presenti in una cartella
 * \bug Ricorsione causa segfault per input errati
 * @param mdir[in] Directory
 * @param len[out] Numero dei file trovati
 * @param depth[in] Profondità di ricerca
 * @return Puntatore alla lista dei file
 */
char **getFiles(char *mdir, int *len, int depth) {
    DIR *dir;
    *len = 0;
    char **out;
    char stringa[2048];
    out = (char **) malloc(1 * sizeof(char *));
    struct dirent *ent;
    if ((dir = opendir(mdir)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
                strcpy(stringa, mdir);
                strcat(stringa, ent->d_name);
                if (is_regular_file(stringa)) {
                    (*len)++;
                    out = (char **) realloc(out, *len * sizeof(char *));
                    if(strlen(ent->d_name) + strlen(mdir) > 0){
                        out[*len - 1] = (char *) malloc(strlen(ent->d_name) + strlen(mdir) * sizeof(char));
                        strcpy(out[*len - 1], mdir);
                        strcpy(out[*len - 1] + (strlen(mdir)), ent->d_name);
                    }
                } else {
//                    strcat(stringa, "/");
//                    if (depth > 0) {
//                        char **rout;
//                        int rlen;
//                        rout = getFiles(stringa, &rlen, depth - 1);
//                        if(rout != NULL) for (int i = 0; i < rlen; i++) printf("===%s\n", rout[i]);
//                        /*if (rlen > 0) {
//                            (*len) += rlen;
//                            out = (char **) realloc(out, *len * sizeof(char *));
//                            for (int i = (*len) - rlen - 1; i < *len; i++) out[i] = rout[i - ((*len) - rlen - 1)];
//                        }*/
//                        free(rout);
//                    }
                }
            }
        }
        closedir(dir);
    }
    return out;
}