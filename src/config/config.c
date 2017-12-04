/**
 * \file config.c
 * \version 1.0
 * \author Stefano
 * \date 07-11-2017
 * \brief Operazioni sui file di configurazione
 */
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include "config.h"

/**
 * \brief Copiatura di una stringa con caratteri speciali
 * La funzione copia la stringa sorgente in un buffer destinatario ignorando i caratteri ' ' e '\n'.
 * @param dest[out] destinazione
 * @param src[in] sorgente
 */
static void mstrcpy(char *dest, const char *src) {
    int i = 0;
    int q = 0;
    while (src[i] != '\0') {
        if (src[i] != ' ' && src[i] != '\n') {
            dest[q++] = src[i] == '~' ? ' ' : src[i];
        }
        i++;
    }
    dest[i] = '\0';
}

/**
 * \brief comparazione di stringhe.
 * La funzione compara due stringhe dati inizio e fine
 * @param str0[in] prima stringa
 * @param str1[in] seconda stringa
 * @param start[in] index di partenza
 * @param finish[in] index di fine
 * @return 0 nel caso le stringhe differiscano, 1 altrimenti
 */
static int mstrcmp(const char *str0, const char *str1, int start, int finish) {
    for (int i = start; i < finish; i++) {
        if (str0[i] != str1[i]) return 0;
    }
    return 1;
}

char **destination;

/**
 * \brief Apre un file di configurazione per la lettura.
 * Viene aperto e letto un file e salvato in memoria in un char **, ignorando
 * le linee di commento (carattere iniziale '#') e le linee vuote.
 * @param outlines[out] Numero di linee lette
 * @param filename[in] file di configurazione
 * @param comment[in] Carattere di commento
 * @return puntatore all'array di linee letto
 */
char ** openConfigFile(int *outlines, const char *filename, const char comment) {
    FILE *conf = fopen(filename, "r");
    if (conf == NULL) return NULL;

    char *line;
    size_t len = 0;
    int lines = 0;
    while (getline(&line, &len, conf) != -1) {
        if (strcmp(line, "_STOP_PARSING_\n") == 0) break;
        if (line[0] != '\x0a' && line[0] != '\r' && line[0] != comment) lines++;
    }

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

/**
 * \brief Ricerca di un carattere in una stringa
 * @param line[in] Stringa in cui cercare.
 * @param ch[in] Carattere da cercare.
 * @param maxLen[in] Lunghezza massima della stringa
 * @return La posizione del carattere nella stringa in caso il carattere sia presente, -1 altrimenti
 */
static int findChar(const char *line, char ch, int maxLen) {
    int q = 0;
    while (q < maxLen && line[q] != '\0') {
        if (line[q] == ch) return q;
        q++;
    }
    return -1;
}

/**
 * \brief Ottenimento di una proprietà dal file di configurazione.
 * La funzione cerca linearmente la proprietà richiesta all'interno del file letto,
 * e, nel caso la proprietà venga trovata, restituisce un puntatore alla stringa
 * definita dalla proprietà nel file.
 * Nel caso la proprietà richiesta sia una lista, viene restituito un puntatore a char
 * con valore uguale alla lunghezza della lista, e l'intero isAList verrà settato a 1.
 * @param file[in] lista delle righe ritornata da openConfigFile \see openConfigFile
 * @param lines[in] Lunghezza della lista
 * @param property[in] Nome della proprietà da ricercare
 * @param lenght[in] Lunghezza della stringa di ritorno
 * @param isAList[out] Se 1 il valore ritornato riguarda una lista, altrimenti 0
 * @param listLen[out] Se isAList == 1, indica la lunghezza della lista
 * @return Un puntatore al valore della lista in caso la proprietà venga trovata, NULL altrimenti.
 */
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

/**
 * \brief Chiusura del file di configurazione
 */
void closeConfigFile(){
    free(destination);
}