//
// Created by stefano on 07/11/17.
//

#ifndef NB_CONFIG_H
#define NB_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_LINE_LEN 1024

char **openConfigFile(int *lines, const char *filename, const char comment);
char *getProperty(char **file, int lines, const char *property, int lenght, int *isAList, int *listLen);
void closeConfigFile();

#ifdef __cplusplus
}
#endif

#endif //NB_CONFIG_H
