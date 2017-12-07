/**
 * \file Headers.h
 * \version 1.0
 * \author Stefano
 * \date 28-10-2017
 * \brief Definizione delle strutture di trasmissione e gestione dei dati
 */
#ifndef NB_HEADERS_H
#define NB_HEADERS_H

#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * La struttura Rappresenta un pacchetto generico ricevuto dal server UDP
 */
typedef struct __attribute__((packed, aligned(4))) {
    char mw[2];        ///< Identificxatore del pacchetto
    char data[17];    ///< Dati presenti
} mainpacket;


typedef mainpacket udpPacketRecv_t;
typedef udpPacketRecv_t identify_h; //id
typedef udpPacketRecv_t askPkey_h;   //pk
typedef udpPacketRecv_t newBak_h;

/**
 * Pacchetto di richiesta identificazione
 */
typedef struct __attribute__((packed, aligned(4))){ //mw = ad
    char mw[2];
    int nextLenght;
} identify_answ_h;

/**
 * Pacchetto di richiesta chiave pubblica
 */
typedef struct __attribute__((packed, aligned(4))){
    int explen;
    int nlen;
} pkey_identifier_t;

/**
 * Header di backup
 */
typedef struct __attribute__((packed, aligned(4))){
    int isEncoded; ///< Se 0 non codificato, altrimenti codificato
    struct sockaddr_in client; ///< Client connesso
    int numberOfFiles; ///< Numero di file nel backup
    time_t time; ///< data di trasmissione
    int packetSize; ///< dimensione di trasmissione
    char foo[32]; ///< dati aggiuntivi
} backupHead_t;

/**
 * Header del file di backup
 */
typedef struct __attribute__((packed, aligned(4))){
    uint64_t dimension; ///< dimensione del file non cifrato
    uint64_t transfer_dimension; ///< dimensione trasmessa
    int isEncoded; ///< se 0 non codificato, altrimenti codificato
    time_t time; ///< data di trasmissione
    char name[256]; ///< nome originale del file
    char foo[32]; ///< dati aggiuntivi
} file_h;


typedef union {
    uint8_t parts[4];
    uint32_t ip;
} ipAddr;

/**
 * Definizione di rete
 */
typedef struct __attribute__((packed, aligned(4))){
    ipAddr address; ///< ip
    unsigned int netMask; ///< netmask
} network;

/**
 * Struttura contenete le configurazioni del server
 */
typedef struct __attribute__((packed, aligned(4))){
    int server_port;    ///< Porta
    int starting_port;    ///< prima porta per i backup
    int port_interval; ///< intervallo di porte
    int ToZip; ///< Se 0 non comprimerà i files, altrimenti sì
    uint64_t maxRamAmount; ///< se un file sarà più piccolo della dimensione specificata verrà compresso
    int allowEqualsDeny_nets; ///< la lista di reti sarà blacklistata
    int allowEqualsDeny_ips; ///< la lista di host sarà blacklistata
    int netsNo; ///< Numero di reti in lista
    network *networks; ///< referenza alla lista di reti
    int ipsNo; ///<< Numero di host in lista
    ipAddr *ips; ///<< referenza alla lista di host
    int manNo; ///< Numero di host amministrativi in lista
    ipAddr *mans; ///< referenza alla lista di amministratori
} conf;

/**
 * Informazioni di backup
 */
typedef struct __attribute__((packed, aligned(4))){
    int socket; ///< socket del processo di backup
    int port; ///< porta di backup
    int pid; ///< pid del processo forkato
    struct sockaddr client; ///< client connesso
    uint64_t dimension; ///< dimensione di trasmissione
    uint64_t transferred; ///< dimensione trasmessa all'istante della lettura
    int numberOfFiles; ///< numero di file totali
    int filesTransferred; ///< numero dfi file trasferiti all'istante di lettura
    time_t startedInTime; ///< data di avvio
    int status; ///< stato (0) running (1) pause
} backupThread;

/**
 *Configurazione SSH
 */
typedef struct __attribute__((packed, aligned(4))){
    int port; ///< porta del terminale ssh
    char *user; ///< username
    char *password; ///< hash della password
    char *rsa; ///< referenza al file con le chiavi rsa
    char *dsa; ///< referenza al file con le chiavi dsa
    int usepcap; ///< se 0 non usare file pcap, altrimenti sì
    char *pcap; ///< referenza al file pcap
} sshconf;

identify_h buildIdentify();
identify_answ_h buildIdentifyAnsw();
askPkey_h buildPkeyPacket();
newBak_h buildBakPacket();

#ifdef __cplusplus
}
#endif

#endif //NB_HEADERS_H
