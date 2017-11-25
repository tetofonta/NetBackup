//
// Created by stefano on 28/10/17.
//
#include "Networking.h"
#include <stdio.h>
int retries = 8;
int delayms = 1000;

void setPrams(const int Cretries, const int Cdelayms) {
    retries = Cretries;
    delayms = Cdelayms;
}

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

int wait4conn(int sockfd, int backlog, struct sockaddr * client, socklen_t * clilen){
    listen(sockfd, backlog);
    int a = accept(sockfd, client, clilen);
    return a;
}

int closeSocket(const int socket) {
    return close(socket);
}