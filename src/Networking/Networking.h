//
// Created by stefano on 28/10/17.
//

#ifndef NB_NETWORKING_H
#define NB_NETWORKING_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <memory.h>

#include "../Compatibility/macros.h"

extern int retries;
extern int delayms;

typedef struct{
    int socket;
    struct sockaddr_in clientData;
    socklen_t clientDataLenght;
} connection_t;

void setPrams(const int Cretries, const int Cdelayms);
int newTCPSocket_client(const int portno, const char *hostName);
int newTCPSocket_server(const int portno);
int closeSocket(const int socket);
connection_t newUDPSocket_client(const int portno, const char *hostName, const int timeout);
int newUDPSocket_server(const int portno);
int wait4conn(int sockfd, int backroll, struct sockaddr * client, socklen_t * clilen);

#endif //NB_NETWORKING_H
