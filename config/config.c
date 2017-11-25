//
// Created by stefano on 07/11/17.
//
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include "config.h"

static void mstrcpy(char *dest, const char *src) {
    int i = 0;
    int q = 0;
    while (src[i] != '\0') {
        if (src[i] != ' ' && src[i] != '\n') {
            dest[q++] = src[i];
        }
        i++;
    }
    dest[i] = '\0';
}

char **destination;

char **openConfigFile(int *outlines, const char *filename, const char comment) {
    FILE *conf = fopen(filename, "r");
    if (conf == NULL) return NULL;

    char *line;
    size_t len = 0;
    int lines = 0;
    while (getline(&line, &len, conf) != -1)
        if (line[0] != '\x0a' && line[0] != '\r' && line[0] != comment) lines++;

    destination = (char **) malloc(lines * sizeof(char *));

    rewind(conf);

    int i = 0;
    while (getline(&line, &len, conf) != -1) {
        if (line[0] != '\x0a' && line[0] != '\r' && line[0] != comment) {
            destination[i] = (char *) malloc(MAX_LINE_LEN * sizeof(char));
            mstrcpy(destination[i], line);
            i++;
        }
    }

    *outlines = lines;

    return destination;
}

static int findChar(const char *line, char ch, int maxLen) {
    int q = 0;
    while (q < maxLen && line[q] != '\0') {
        if (line[q] == ch) return q;
        q++;
    }
    return -1;
}

static int mstrcmp(const char *str0, const char *str1, int start, int finish) {
    for (int i = start; i < finish; i++) {
        if (str0[i] != str1[i]) return 0;
    }
    return 1;
}

char * getProperty(char **file, int lines, const char *property, int lenght, int * isAList, int * listLen) {
    *isAList = 0;
    for (int i = 0; i < lines; i++) {
        int equal = findChar(file[i], '=', lenght);
        if (equal == -1) continue;
        if (mstrcmp(file[i], property, 0, equal)) {
            if (*(file[i] + equal + 1) == ':') {
                *isAList = 1;
                int n = i;
                while(file[n][0] != ':' && file[n][1] != ':') n++;
                n -= i+1;
                *listLen = n;
                return (char *) i+1;
            } else return file[i] + equal + 1;
        }
    }
    return NULL;
}

void closeConfigFile(){
    free(destination);
}