/**
 * \file bakInfo.c
 * \version 1.0
 * \author Stefano
 * \date 28-11-2017
 * \brief Utility di infrmazione
 */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Headers.h"

char file[4096];

/**
 * \brief Stampa le informazioni relative ad un file hd
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char ** argv){
    if(argc < 2){
        printf("Usage ./bakinfo <directory di backup>");
        return -6;
    }

    strcpy(file, argv[1]);
    if(file[strlen(file)-1] != '/' && file[strlen(file)-1] != '\\') strcat(file, "/");

    strcat(file, "head.hd");

    FILE * head = fopen(file, "r");
    if(head == NULL) return -1;

    backupHead_t hd;
    fread(&hd, sizeof(hd), 1, head);

    char res[100];
    struct tm lt;
    time_t t = time(NULL);
    t = hd.time;
    (void) localtime_r(&t, &lt);
    if(strftime(res, sizeof(res), "%a %b %d %Y", &lt) == 0){
        bzero(res, 100);
        strcpy(res, "NOT-DEFINED");
    }

    printf("BACKUP INFO PER ==%s== :\n"
                   "Cifrato AES 256: %s\n"
                   "Possessore: %s\n"
                   "Totale file: %d\n"
                   "In data: %s\n"
                   "Dimensione di trasferimento: %d\n", file, hd.isEncoded ? "SI" : "NO", inet_ntoa(hd.client.sin_addr),
    hd.numberOfFiles, res, hd.packetSize);

    fclose(head);
    return 0;
}