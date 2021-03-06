/**
 * \file terminal.c
 * \version 1.0
 * \author Stefano
 * \date 18-11-2017
 * \brief Gestione terminale
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <dlfcn.h>

#include "terminal.h"
#include "../MD5/md5.h"

#ifdef GMP
#include "../RSA/RSA.h"
#endif

char bf[2048];
char * bufffer;
char * elf;
char md[16];
char fooo[33];

/**
 * \brief Identifica il comando
 * Viene trovato il comando, se esiste viene controllata la firma digitale con i parametri pubblici in memoria
 * e viene quindi eseguito il codice.
 * @param command[in] Stringa del comando con gli argomenti
 * @param len[in] Lunghezza della stringa del comando
 * @param baks[in] Referenza al vettore di processi di backup \see server.cpp
 * @param cfgs[in] Referenza alla cconfigurazione del server
 * @param printToStream[in] Funzione di stampa
 */
void parseCommand(char * command, size_t len, backupThread * baks, conf * cfgs, int (*printToStream)(const char * format, ...)){

    sprintf(bf, "%s ", command);
    strcpy(command, bf);

    int i = 0;
    while(i < strlen(command) && command[i++] != ' ');
    command[i-1] = '\0';

#ifdef GMP

    for(int q = 0; q < strlen(command); q++) if(command[q] == ' ') command[q] = '\0';

    bzero(bf, 2048);
    sprintf(bf, "./comandi/%s", command);

    loadpkey("CA.public", 1);

    FILE * fp = fopen(bf, "rb");
    if(fp == NULL){
        printToStream(">>>Command %s unavailable<<<\n", command);
        return;
    }

    fseek(fp, -4, SEEK_END);

    int signlen;
    fread(&signlen, sizeof(int), 1, fp);

    fseek(fp, -4-signlen, SEEK_END);
    bufffer = (char *) malloc(signlen*sizeof(char)+1);
    fread(bufffer, 1, static_cast<size_t>(signlen), fp);
    bufffer[signlen] = '\0';

    fseek(fp, -4-signlen, SEEK_END);
    size_t elflen = static_cast<size_t>(ftell(fp));

    rewind(fp);

    elf = (char *) malloc(elflen * sizeof(char)+1);
    fread(elf, 1, elflen, fp);
    md5((uint8_t *) elf, elflen, md);

    for(int q = 0; q < 16; q++) sprintf(fooo + (q*2), "%2.2x", md[q] & 0xFF);
    fooo[32] = '\0';

    mpz_t check;
    mpz_init_set_str(check, bufffer, 16);
    unsign(check, check, 1);
    char *returned = mpz_get_str(NULL, 16, check);
    mpz_clear(check);

    fclose(fp);
    int start = 0;
    while((start + 1) && fooo[start++] == '0');
    start--;
    if(strcmp(returned, fooo+start)){
        printToStream("Eseguibile non verificabile - hash error\n");
		printToStream("\n%s\n%s\n", returned, fooo);
        return;
    }

    free(bufffer);
    free(elf);
#else
    printToStream("Impossibile verificare l'attendibilità dell'eseguibile, nessuna libreria installata");
#endif
    int (*execute)(char *, const backupThread *, const conf *, int (*)(const char *, ...));
    void * handle = dlopen(bf, RTLD_NOW);
    if(!handle){
        printToStream(">>>Command %s unavailable<<<\n", command);
        return;
    }

    dlerror();

    *(void **) (&execute) = dlsym(handle, "execute");

    char * error = dlerror();
    if (error != NULL) {
        printToStream("\n=========%s\n==========\n", error);
        return;
    }


    int ret = (*execute)(command + i , baks, cfgs, printToStream);

    dlclose(handle);

    printToStream("\n(%d)\n", ret);
}

/**
 * \brief Inizializzazione del termiale
 * @param serverpid[in] PID del server
 * @param baks[in] Referenza al vettore di processi di backup \see server.cpp
 * @param cfgs[in] Referenza alla cconfigurazione del server
 */
void init_terminal(pid_t serverpid, backupThread * baks, conf * cfgs) {
    char * command = NULL;
    size_t len;

    do{
        printf(">> "); fflush(stdout);
        getline(&command, &len, stdin);
        strtok(command, "\n");
        if(strcmp(command, "exit") != 0){
            parseCommand(command, len, baks, cfgs, printf);
        }
    } while(strcmp(command, "exit") != 0);

    kill(serverpid, SIGHUP);
}