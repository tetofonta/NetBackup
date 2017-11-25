//
// Created by stefano on 28/10/17.
//
#include "Networking.h"
#include <stdio.h>
#include <sys/time.h>
connection_t newUDPSocket_client(const int portno, const char *hostName, const int timeout) {
    connection_t foo;
    foo.socket = -1;
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in serv_addr;
    int servlen = sizeof(serv_addr);
    struct hostent *server;

    if (sockfd < 0) return foo;

    bzero(&serv_addr, (size_t) servlen);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons((uint16_t) portno);

    if(inet_aton(hostName, &serv_addr.sin_addr) == 0) return foo;

    foo.socket = sockfd;
    foo.clientData = serv_addr;
    foo.clientDataLenght = (socklen_t) servlen;

    struct timeval tv;
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
        foo.socket = -1;

    return foo;
}

int newUDPSocket_server(const int portno) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in serv_addr;
    int servlen = sizeof(serv_addr);
    struct hostent *server;

    if (sockfd < 0) return -1;

    bzero(&serv_addr, (size_t) servlen);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons((uint16_t) portno);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sockfd, (struct sockaddr *)&serv_addr, (socklen_t) servlen) < 0) return -2;

    return sockfd;
}