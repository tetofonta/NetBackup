//
// Created by stefano on 07/11/17.
//

#ifndef NB_CONFIG_H
#define NB_CONFIG_H

#define MAX_LINE_LEN 1024

char ** openConfigFile(int * lines, const char *filename, const char comment);
char * getProperty(char ** file, int lines, const char * property, int lenght, int * isAList, int * listLen);
void closeConfigFile();

#endif //NB_CONFIG_H
