/**
 * \file udp.c
 * \version 1.0
 * \author Stefano
 * \date 28-11-2017
 * \brief Funzioni di rete UDP
 */
#include <stdio.h>
#include "Networking.h"
int retries = 8;
int delayms = 1000;

/**
 * \brief Imposta i parametri di connessione
 * @param Cretries[in] Tentativi massimi di connessione
 * @param Cdelayms[in] Intervallo tra due tentativi
 */
void setPrams(const int Cretries, const int Cdelayms) {
    retries = Cretries;
    delayms = Cdelayms;
}

/**
 * \brief Connette la macchina ad un server TCP
 * @param portno[in] numero della porta di connessione
 * @param hostName[in] Nome dell'host o IPv4
 * @return Descrittore del socket
 */
int newTCPSocket_client(const int portno, const char *hostName) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (sockfd < 0) return -1;

    server = gethostbyname(hostName);
    if (server == NULL) return -2;

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
    serv_addr.sin_port = htons((uint16_t) portno);

    for (int i = 0; i < retries; i++){
        if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 || sockfd < 0){

            _SLEEP(delayms);
        }
        else return sockfd;
    }

    perror("Attemp failed: ");

    return -42;
}

/**
 * \brief Crea un server TCP
 * @param portno[in] Porta dove stanziare il server
 * @return Intero descrittore del socket
 */
int newTCPSocket_server(const int portno) {
    int sockfd;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return -1;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons((uint16_t) portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) return -2;
    return sockfd;
}

/**
 * \brief Attende una connessione
 * La funzione blocca l'esecuzione del codice fino a che un client si connette al server tcp
 * @param sockfd[in] Descrittore del server TCP
 * @param backlog[in] Numero massimo di connessioni in coda prima di inviare 'ERR CONNECTION REFUSED'
 * @param client[out] Struttura relativa al client connesso
 * @param clilen[out] Lunghezza della struttura precedente
 * @return
 */
int wait4conn(int sockfd, int backlog, struct sockaddr * client, socklen_t * clilen){
    listen(sockfd, backlog);
    int a = accept(sockfd, client, clilen);
    return a;
}

/**
 * \brief Chiude un socket
 * @param socket[in] Descrittore del socket
 * @return 0 in caso di riuscita, -1 altrimenti
 */
int closeSocket(const int socket) {
    return close(socket);
}