//
// Created by stefano on 25/11/17.
//

#ifndef NB_FS_H
#define NB_FS_H

#ifdef __cplusplus
extern "C" {
#endif

int is_regular_file(const char *path);
char **getFiles(char *mdir, int *len, int);

#ifdef __cplusplus
}
#endif

#endif //NB_FS_H
