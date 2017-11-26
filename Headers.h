//
// Created by stefano on 29/10/17.
//

#ifndef NB_HEADERS_H
#define NB_HEADERS_H

#include <netinet/in.h>
typedef struct{
    char mw[2];
    char data[17];
}__attribute__((packed)) mainpacket;


typedef mainpacket udpPacketRecv_t;
typedef udpPacketRecv_t identify_h; //id
typedef udpPacketRecv_t askPkey_h;   //pk
typedef udpPacketRecv_t newBak_h;

typedef struct{ //mw = ad
    char mw[2];
    int nextLenght;
}__attribute__((packed)) identify_answ_h;

typedef struct{
    int explen;
    int nlen;
}__attribute__((packed)) pkey_identifier_t;

typedef struct{
    int isEncoded;
    sockaddr_in client;
    int numberOfFiles;
    time_t time;
    int packetSize;
    char foo[32];
}__attribute__((packed)) backupHead_t;

typedef struct{
    uint64_t dimension;
    uint64_t transfer_dimension;
    int isEncoded;
    time_t time;
    char name[50];
    char foo[32];
}__attribute__((packed)) file_h;


typedef union {
    uint8_t parts[4];
    uint32_t ip;
}ipAddr ;

typedef struct {
    ipAddr address;
    unsigned int netMask;
}network ;


typedef struct{
    int server_port;
    int starting_port;
    int port_interval;
    int ToZip;
    uint64_t maxRamAmount;
    int allowEqualsDeny_nets;
    int allowEqualsDeny_ips;
    int netsNo;
    network * networks;
    int ipsNo;
    ipAddr * ips;
    int manNo;
    ipAddr * mans;
}__attribute__((packed)) conf;

typedef struct {
    int socket;
    int port;
    sockaddr client;
    uint64_t dimension;
    uint64_t transferred;
    int numberOfFiles;
    int filesTransferred;
    time_t startedInTime;
    int status;
}__attribute__((packed)) backupThread;

typedef struct{
    int port;
    char * user;
    char * password;
    char * rsa;
    char * dsa;
    int usepcap;
    char * pcap;
}__attribute__((packed)) sshconf;

identify_h buildIdentify();
identify_answ_h buildIdentifyAnsw();
askPkey_h buildPkeyPacket();
newBak_h buildBakPacket();

#endif //NB_HEADERS_H
