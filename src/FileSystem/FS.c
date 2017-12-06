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
#include "../Compatibility/macros.h"

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
    out = (char **) malloc(1 * sizeof(char *));
    char stringa[2048];
    struct dirent *ent;
    if ((dir = opendir(mdir)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                strcpy(stringa, mdir);
                strcat(stringa, ent->d_name);
                if (is_regular_file(stringa)) {
                    (*len)++;
                    out = (char **) realloc(out, *len * sizeof(char *));
                    if(strlen(ent->d_name) + strlen(mdir) > 0){
                        out[*len - 1] = (char *) malloc((strlen(ent->d_name) + strlen(mdir) + 20)* sizeof(char));
                        strcpy(out[*len - 1], mdir);
                        strcpy(out[*len - 1] + (strlen(mdir)), ent->d_name);
                    }
                } else {
//                    strcat(stringa, "/");
//                    if (depth > 0) {
//                        char **rout;
//                        int rlen;
//                        rout = getFiles(stringa, &rlen, depth - 1);
//                        if(rout != NULL) {
//                            if (rlen > 0) {
//                                out = (char **) realloc(out, ((*len)+rlen * sizeof(char *)));
//                                printf("REALLOCATED %d %d %s\n", *len, rlen, stringa);
//                                for (int i = 0; i < rlen; i++) {
//                                    out[i+*len] = (char *) malloc(strlen(rout[i]) * sizeof(char));
//                                    //strcpy(out[i], rout[i - ((*len) - rlen - 1)]);
//                                }
//                                *len = *len + rlen;
//                            }
//                        }
//                        free(rout);
//                    }
                }
            }
        }
        closedir(dir);
    }
    return out;
}