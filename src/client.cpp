/**
 * \file client.cpp
 * \version 1.0
 * \author Stefano
 * \date 28-10-2017
 * \brief Client
 */
#define _FILE_OFFSET_BITS 64

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cinttypes>
#include <csignal>
#include "Networking/Networking.h"
#include "BASE_encoding/base32.h"
#include "FileSystem/FS.h"

#ifdef GMP
#include "RSA/RSA.h"
#include "RSA/internal.h"
#endif

#include "Headers.h"
#include "AES/Aes.h"
#include "config/config.h"



int server_port;
char *server_ip;
int isEncrypted;
int timeout;
char **files;
int fileNumbers = 0;
int transfer_block_size;
char key[32];
int interval;

uint8_t * data;

ssize_t recv_tcp(int socket, void * dest, size_t size, int flag){
    size_t recived = 0;
    while(size > recived){
        ssize_t rc = recv(socket, dest + recived, size-recived, flag);
        if(rc < 0)
            return rc;
        recived += rc;
    }
    return recived;
}

/**
 * \brief Ritorna una stringa relativa a una proprietà oppure una lista
 * @param file[in] File descrittore della configurazione
 * @param lines[in] lunghezza di file
 * @param prop[in] Nome della proprietà
 * @param len[in] Lunghezza massima
 * @return Puntatore a una stringa con il valore, NULL se è una lista
 */
static char *sanitizeValue(char **file, int lines, const char *prop, int len) {
    int isList, llen;
    char *ret = getProperty(file, lines, prop, len, &isList, &llen);
    if (!isList) return ret;
    return NULL;
}

/**
 * \brief atoi() con controllo per puntatore null
 * @param str
 * @return
 */
int atoi_sec(char * str){
    if(str == NULL) return -1;
    return atoi(str);
}

char **file;
/**
 * \brief Caricamento della configurazione da file
 * @param configFile[in] Nome del file di configurazione
 * @return 0 in caso di riuscita, -1 altrimenti
 * \bug In caso una proprietà non sia presente segfault
 */
int loadCfg(char * configFile) {
    int lines;
    if ((file = openConfigFile(&lines, configFile, '#')) == NULL) return -1;
    int isList = 0, len = 0;

    int interval = atoi_sec(sanitizeValue(file, lines, "send_interval", MAX_LINE_LEN));

    server_ip = sanitizeValue(file, lines, "server_ip", MAX_LINE_LEN);
    server_port = atoi_sec(sanitizeValue(file, lines, "server_port", MAX_LINE_LEN));

    timeout = atoi_sec(sanitizeValue(file, lines, "connection_timeout", MAX_LINE_LEN));
    isEncrypted = atoi_sec(sanitizeValue(file, lines, "encrypt", MAX_LINE_LEN));
    transfer_block_size = 16 * atoi_sec(sanitizeValue(file, lines, "transfer_block_size", MAX_LINE_LEN));
    char * obtainedKey = sanitizeValue(file, lines, "key", MAX_LINE_LEN);
    char * obtk = (char *) malloc(500);
    base32_decode((const unsigned char *) obtainedKey, (unsigned char *) obtk);

    bzero(key, 32);
    for(int i = 0; i < 32; i++) key[i] = obtk[i];

    char * ret = getProperty(file, lines, "files", MAX_LINE_LEN, &isList, &len);
    if(isList){
        int index = (int)(long)ret;
        int curlen = 0;
        files = (char **) malloc(1);
        for(int i = index; i < index + len; i++){
            if(is_regular_file(file[i])) {
                //printf("Adding %s\n", file[i]);
                files = (char **) realloc(files, ++curlen*sizeof(char *));
                files[curlen-1] = (char *) malloc(MAX_LINE_LEN * sizeof(char));
                strcpy(files[i - index], file[i]);
            } else {
                //printf("Dir %s\n", file[i]);
                char ** out;
                int lenght;
                out = getFiles(file[i], &lenght, 2);
                curlen += lenght;
                void * foo = realloc(files, curlen*sizeof(char *));
                files = (char **) foo;

                for(int q = 0; q < lenght; q++){
                    files[curlen - (lenght - q)] = out[q];
                }
            }
        }
        fileNumbers = curlen;
    }

    //for(int i = 0; i < fileNumbers; i++) printf("==%s\n", files[i]);

    closeConfigFile();

    return 0;
}

/**
 * \brief Genera un carattere casuale
 * @return Un carattere casuale
 */
static char rndChar() {
    char possibili[] = "abcdef01234567890";
    return possibili[rand() % 16];
}

/**
 * \brief Richiesta di identità del server
 * Viene richiesta al server la firma digitale di una stringa casuale generata dal client, quindi viene verificata mediante la chiave memorizzata in memoria.
 * @param socket[in] struttura parametri di connessione
 * @return -10 in caso il file relativo al server non esista o non sia accessibile, 1 in caso di verifica esatta, 0 altriemeti
 */
int askIdentity(connection_t socket) {
#ifdef GMP
    char foo[35];
    sprintf(foo, "archive/%s.public", server_ip);
    loadpkey(foo, 0);

    if (errorHappened) return -10;

    identify_answ_h answ;
    socklen_t len = (socklen_t) sizeof(answ);

    identify_h pk = buildIdentify();
    for (int i = 0; i < 16; i++) pk.data[i] = rndChar();
    pk.data[16] = 0x00;

    sendto(socket.socket, &pk, sizeof(pk), 0, (const sockaddr *) &socket.clientData, socket.clientDataLenght);
    recvfrom(socket.socket, &answ, sizeof(answ) - 1, 0, NULL, &len);
    char number[answ.nextLenght + 1];
    recvfrom(socket.socket, &number, (size_t) answ.nextLenght, 0, NULL, 0);
    number[answ.nextLenght] = '\0';

    mpz_t check;
    mpz_init_set_str(check, number, 16);
    unsign(check, check, 0);
    char *returned = mpz_get_str(NULL, 16, check);
    mpz_clear(check);

    if (strcmp(returned, pk.data) == 0) {
        free(returned);
        return 1;
    }
    free(returned);
    return 0;
#else
    return 1;
#endif
}

/**
 * \brief Salva una chiave pubblica in memoria
 * La funzione richiederà al server la sua chiave pubblica e la memorizzerà in un file .public in memoria.
 * @param server[in] struttura parametri di connessione.
 */
void savePkey(connection_t server) {
    printf("Requiring server identity...\n");
    askPkey_h quest = buildPkeyPacket();
    sendto(server.socket, &quest, sizeof(quest), 0, (const sockaddr *) &server.clientData, server.clientDataLenght);

    pkey_identifier_t id;
    recvfrom(server.socket, &id, sizeof(id), 0, NULL, 0);

    char *n_str = (char *) malloc(id.nlen * sizeof(char) + 10);
    bzero(n_str, id.nlen * sizeof(char) + 10);
    char *e_str = (char *) malloc(id.explen * sizeof(char) + 10);
    bzero(e_str, id.explen * sizeof(char) + 10);

    recvfrom(server.socket, n_str, (size_t) id.nlen, 0, NULL, 0);
    recvfrom(server.socket, e_str, (size_t) id.explen, 0, NULL, 0);

    char foo[35];
    sprintf(foo, "archive/%s.public", server_ip);
    FILE *boh = fopen(foo, "w");
    if (boh == NULL) return;
    fprintf(boh, "%s\n%s", n_str, e_str);
    fclose(boh);

    free(n_str);
    free(e_str);
}

/**
 * \brief La funzione inizia la trasmissione di un nuovo backup
 * @param port[in] Porta di comunicazione con il server tramite protocollo TCP
 */
int newBak(int port) {
    int tcp = newTCPSocket_client(port, server_ip);
    _SLEEP(500);
    backupHead_t header;
    header.isEncoded = isEncrypted;
    header.numberOfFiles = fileNumbers;
    header.packetSize = transfer_block_size;
    header.time = time(NULL);
    if(send(tcp, &header, sizeof(header), 0) < sizeof(header)) return -1000;
    for(int i = 0; i < fileNumbers; i++){

        //_SLEEP(3000);

        file_h fileH;
        fileH.time = time(NULL);
        fileH.isEncoded = isEncrypted;

        //if(strcmp(files[i], "") == 0 || files[i] == NULL) continue;

        FILE * fp = fopen(files[i], "r");

        if(fp == NULL){
            printf("Cannot open file '%s'\n", files[i]);
            fflush(stdout);
            continue;
        }
        fseeko(fp, 0L, SEEK_END);
        fileH.dimension = (uint64_t) ftello(fp);
        fileH.transfer_dimension = fileH.dimension;
        if(isEncrypted){
            fileH.transfer_dimension = fileH.dimension + ( transfer_block_size - (fileH.dimension % transfer_block_size));
        }
        rewind(fp);


        int lastSlash = 0;
        int n = 0;
        while(files[i][n] != '\0'){
            if(files[i][n] == '\\' || files[i][n] == '/') lastSlash = n;
			if(files[i][n] == ' ') files[i][n] = '_';
            n++;
        }
		int copied = 0;
		while(copied < 256 && files[i][copied + lastSlash + 1] != '\0'){
			fileH.name[copied] = files[i][copied + lastSlash + 1];
			copied++;
		}
        strcpy(fileH.name + copied, (copied == 45) ? "~.bak" : ".bak");

        if(send(tcp, &fileH, sizeof(file_h), 0) < 0){
            perror("TRANSMISSION ");
            continue;
        };

        printf("Transferring %" PRId64 " bytes of file %s\n", fileH.transfer_dimension, files[i]);
        int totalReaded = 0;
        while(!feof(fp)){
            _SLEEP(interval);
            if(isEncrypted) bzero(data, transfer_block_size*sizeof(uint8_t));
            size_t readed = fread(data, sizeof(uint8_t), (size_t) transfer_block_size, fp);

            /*for(int j = 0; j < readed; j++) printf("%2.2x ", data[j] & 0xFF);
            printf("\n");*/

            if(isEncrypted){
                for(int q = 0; q < transfer_block_size/16; q++) {

                    AES_ECB_encrypt(data + (q * 16), (uint8_t *) key, data + (q * 16), 16);

                }
                readed = (size_t) transfer_block_size;
            }
            send(tcp, data, readed, 0);

            totalReaded += readed;

            printf("\e[?25l");
            fflush(stdout);

            printf("%d / %" PRId64 "\r", totalReaded, fileH.transfer_dimension);
			
        }
        printf("\e[?25h \n");
        fflush(stdout);
    }

    closeSocket(tcp);
    return 0;
}

char configF[512];
int main(int argc, char ** argv) {
    signal(SIGPIPE, SIG_IGN);

    if(argc == 1) strcpy(configF, "client.properties");
    else strcpy(configF, argv[1]);

    if (loadCfg(configF) < 0) return 42;

    data = (uint8_t *) malloc(transfer_block_size * sizeof(uint8_t));

    connection_t server;
    server = newUDPSocket_client(server_port, server_ip, timeout);
    if (server.socket < 0) return 41;

    printf("checking server identity...\n");
    int res = askIdentity(server);

    if (res > 0)
        goto perfect;
    else {
        printf("CANNOT VERIFY SERVER IDENTITY: Continue? [y/N] ");
        char an = (char) getchar();
        if (an != 'y') return 40;
        if (res == -10)
            savePkey(server);
    }

    perfect:
    newBak_h headerr = buildBakPacket();
    headerr.data[0] = (char) isEncrypted;
    sendto(server.socket, &headerr, sizeof(headerr), 0, (const sockaddr *) &server.clientData, server.clientDataLenght);
    int port = -1;

    recvfrom(server.socket, &port, sizeof(int), 0, NULL, 0);
    if (port == -1) return -10;

    int a = newBak(port);

    closeSocket(server.socket);

    //free(data);
    //free(server_ip);
    return a;
}
