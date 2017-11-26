//
// Created by stefano on 28/10/17.
//
#include <cstdlib>
#include <signal.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <stdio.h>
#include <time.h>

#ifdef BZIP
#include <bzlib.h>
#endif

#ifdef GMP
#include "RSA/RSA.h"
#include "RSA/internal.h"
#endif

#include <fcntl.h>
#include "Networking/Networking.h"
#include "Headers.h"
#include "config/config.h"
#include "commands.h"
#include "Terminal/terminal.h"

#ifndef _BZLIB_H
#define BZFILE FILE
#endif

int server;

network *allowedNetworks;
ipAddr *allowedHosts;
ipAddr *managers;

conf configs;
sshconf sshconfigs;
int usessh = 0;

char *rsacfg;

static volatile int keepRunning = 1;

void intHandler(int dummy) {
    keepRunning = 0;
    shutdown(server, SHUT_RDWR);
}

backupThread *processi;

char **file;

static char *sanitizeValue(char **file, int lines, const char *prop, int len) {
    int isList, llen;
    char *ret = getProperty(file, lines, prop, len, &isList, &llen);
    if (!isList) return ret;
    return NULL;
}

int loadCfg() {
    int lines;
    if ((file = openConfigFile(&lines, "config.properties", '#')) == NULL) return -1;
    int isList = 0, len = 0;

    configs.server_port = atoi(sanitizeValue(file, lines, "port", MAX_LINE_LEN));
    configs.starting_port = atoi(sanitizeValue(file, lines, "startingPort", MAX_LINE_LEN));
    configs.port_interval = atoi(sanitizeValue(file, lines, "interval", MAX_LINE_LEN));
    rsacfg = sanitizeValue(file, lines, "rsaConfig", MAX_LINE_LEN);
    configs.ToZip = atoi(sanitizeValue(file, lines, "zipFiles", MAX_LINE_LEN));
    configs.maxRamAmount = (uint64_t) atol(sanitizeValue(file, lines, "maxRamAmount", MAX_LINE_LEN));
    configs.allowEqualsDeny_nets = atoi(sanitizeValue(file, lines, "networks_blacklist", MAX_LINE_LEN));
    configs.allowEqualsDeny_ips = atoi(sanitizeValue(file, lines, "ips_blacklist", MAX_LINE_LEN));

    /*liste*/
    char *ret = getProperty(file, lines, "networks", MAX_LINE_LEN, &isList, &len);
    if (isList) {
        int index = (int) (long) ret;
        allowedNetworks = (network *) malloc(len * sizeof(network));
        configs.netsNo = len;
        for (int i = index; i < len + index; i++) {
            int c[4];
            sscanf(file[i], "%d.%d.%d.%d/%d", &c[0], &c[1], &c[2], &c[3], &(allowedNetworks[i - index].netMask));

            allowedNetworks[i - index].address.parts[0] = (uint8_t)(c[0] & 0xFF);
            allowedNetworks[i - index].address.parts[1] = (uint8_t)(c[1] & 0xFF);
            allowedNetworks[i - index].address.parts[2] = (uint8_t)(c[2] & 0xFF);
            allowedNetworks[i - index].address.parts[3] = (uint8_t)(c[3] & 0xFF);
        }
        configs.networks = allowedNetworks;
    }

    ret = getProperty(file, lines, "ips", MAX_LINE_LEN, &isList, &len);
    if (isList) {
        int index = (int) (long) ret;
        allowedHosts = (ipAddr *) malloc(len * sizeof(ipAddr));
        configs.ipsNo = len;
        for (int i = index; i < len + index; i++) {
            int c[4];
            sscanf(file[i], "%d.%d.%d.%d", &c[0], &c[1], &c[2], &c[3]);

            allowedHosts[i - index].parts[0] = (uint8_t)(c[0] & 0xFF);
            allowedHosts[i - index].parts[1] = (uint8_t)(c[1] & 0xFF);
            allowedHosts[i - index].parts[2] = (uint8_t)(c[2] & 0xFF);
            allowedHosts[i - index].parts[3] = (uint8_t)(c[3] & 0xFF);
        }
        configs.ips = allowedHosts;
    }

    ret = getProperty(file, lines, "managers", MAX_LINE_LEN, &isList, &len);
    if (isList) {
        int index = (int) (long) ret;
        managers = (ipAddr *) malloc(len * sizeof(ipAddr));
        for (int i = index; i < len + index; i++) {
            int c[4];
            sscanf(file[i], "%d.%d.%d.%d", &c[0], &c[1], &c[2], &c[3]);

            managers[i - index].parts[0] = (uint8_t)(c[0] & 0xFF);
            managers[i - index].parts[1] = (uint8_t)(c[1] & 0xFF);
            managers[i - index].parts[2] = (uint8_t)(c[2] & 0xFF);
            managers[i - index].parts[3] = (uint8_t)(c[3] & 0xFF);
        }

        configs.mans = managers;
    }

    usessh = atoi(sanitizeValue(file, lines, "use_ssh", MAX_LINE_LEN));
    if(usessh){
        sshconfigs.port = atoi(sanitizeValue(file, lines, "ssh_port", MAX_LINE_LEN));
        sshconfigs.user = sanitizeValue(file, lines, "ssh_user", MAX_LINE_LEN);
        sshconfigs.password = sanitizeValue(file, lines, "ssh_psw_md5", MAX_LINE_LEN);
        sshconfigs.rsa = sanitizeValue(file, lines, "ssh_rsa", MAX_LINE_LEN);
        sshconfigs.dsa = sanitizeValue(file, lines, "ssh_dsa", MAX_LINE_LEN);
    }

    closeConfigFile();

    return 0;
}

void sendPkey(int socket, sockaddr to, socklen_t len) {
#ifdef GMP
    pkey_identifier_t id;
    char *n_str = mpz_get_str(NULL, 16, rsaPrams[2]);
    char *e_str = mpz_get_str(NULL, 16, rsaPrams[3]);

    id.nlen = (int) strlen(n_str);
    id.explen = (int) strlen(e_str);

    sendto(socket, &id, sizeof(id), 0, &to, len);
    sendto(socket, n_str, (size_t) id.nlen, 0, &to, len);
    sendto(socket, e_str, (size_t) id.explen, 0, &to, len);

    free(n_str);
    free(e_str);
#endif
}

void sendIdentification(int socket, sockaddr to, socklen_t len, identify_h data) {
#ifdef GMP
    identify_answ_h answ = buildIdentifyAnsw();
    data.data[16] = '\0';

    mpz_t tosign;
    mpz_init_set_str(tosign, data.data, 16);
    sign(tosign, tosign);
    char *foo = mpz_get_str(NULL, 16, tosign);
    answ.nextLenght = (int) strlen(foo);
    mpz_clear(tosign);

    sendto(server, &answ, sizeof(answ), 0, &to, len);
    sendto(server, foo, (size_t) answ.nextLenght, 0, &to, sizeof(to));
#endif
}

int getSlot(backupThread *threads, int port_ext) {
    for (int i = 0; i < port_ext; i++)
        if (threads[i].socket < -1) return i;

    return -1;
}

void newBackup(int socket, sockaddr to, socklen_t len, newBak_h data) {

    int slot = getSlot(processi, configs.port_interval);
    if (slot < 0) return;

    int port = processi[slot].port;
    sendto(socket, &port, sizeof(int), 0, &to, len);
    processi[slot].socket = -1;
    processi[slot].client = to;

    pid_t pid = fork();
    if (pid < 0 || pid != 0) return;

    /*child process*/
    processi[slot].startedInTime = time(NULL);
    sockaddr_in *foo = (struct sockaddr_in *) &processi[slot].client;
    printf("Backup started: PID %d, CLIENT %s on slot %d/%d\n", getpid(), inet_ntoa(foo->sin_addr), slot + 1,
           configs.port_interval);

    sockaddr a;
    socklen_t b = sizeof(a);

    int atp = 0;
    int tcps;
    do{
        tcps = newTCPSocket_server(processi[slot].port);
        printf("Opened socket at attemp %d: %d\n", atp+1, tcps);
        if(atp > 0) _SLEEP(500);
    } while(tcps < 0 && atp++ < 5);
    if(atp == 5){
        perror("Opening socket error: ");
        return;
    }

    int reciver;
    reciver = wait4conn(tcps, 1, &a, &b);


    sockaddr_in q = *((sockaddr_in *)&a);

    backupHead_t header;
    recv(reciver, &header, sizeof(header), 0);
    perror("Header recived: ");

    processi[slot].numberOfFiles = header.numberOfFiles;
    processi[slot].filesTransferred = 0;

    header.client = *foo;
    uint8_t *r_data = (uint8_t *) malloc(header.packetSize * sizeof(uint8_t));

    struct stat st = {0};
    char name[50];
    char name_con_nome[58];
    int attemps = 20;
    while (attemps > 0) {
        sprintf(name, "BACKUPS/%s_%ld", inet_ntoa(foo->sin_addr), time(NULL));
        if (stat(name, &st) == -1) {
            mkdir(name, 0760);
            break;
        }
        _SLEEP(1100);
        attemps--;
    }
    sprintf(name_con_nome, "%s/head.hd", name);
    FILE *hd = fopen(name_con_nome, "w");
    if (hd == NULL) {
        perror("backup");
        goto term;
    }
    fwrite(&header, sizeof(backupHead_t), 1, hd);
    fclose(hd);

    for (int i = 0; i < header.numberOfFiles; i++) {
        while(processi[slot].status) _SLEEP(1000);
        file_h fileHeader;
        if (recv(reciver, &fileHeader, sizeof(file_h), 0) <= 0) break;
        processi[slot].dimension = fileHeader.transfer_dimension;
        processi[slot].transferred &= 0;

        int zip = configs.ToZip;
        if (fileHeader.transfer_dimension > configs.maxRamAmount) zip = 0;
#ifndef BZIP
        zip = 0;
#endif

        char fooo[100];
        sprintf(fooo, "%s/%s%s", name, fileHeader.name, zip ? ".bz2" : "");
        printf("Saving %s \n", fooo);

		

        FILE *fp = fopen(fooo, "a");
        BZFILE *zip_p;
        int err;
        if (zip) {
#ifdef BZIP
            zip_p = BZ2_bzWriteOpen(&err, fp, 9, 1, 30);
            if (err != BZ_OK) return;
#endif
        }

        if (fp == NULL) {
            perror("FILE");
            continue;
        }

        if (zip){
#ifdef BZIP
            BZ2_bzWrite(&err, zip_p, &fileHeader, sizeof(fileHeader));
#endif
        }
        else fwrite(&fileHeader, sizeof(fileHeader), 1, fp);

        size_t nextBlock = (size_t) header.packetSize;
        while (fileHeader.transfer_dimension > (uint64_t) 0L) {
            if (fileHeader.transfer_dimension < (uint64_t) 512L) {
                nextBlock = fileHeader.transfer_dimension & 0xFFFFFFFF;
                fileHeader.transfer_dimension &= (uint64_t) 0L;
            }

            size_t reciv = (size_t) recv(reciver, r_data, nextBlock, 0);
            if (reciv <= 0) break;

            if (zip){
#ifdef BZIP
                BZ2_bzWrite(&err, zip_p, r_data, reciv * sizeof(uint8_t));
#endif
            }
            else {
                fwrite(r_data, 1, reciv, fp);
                fflush(fp);
            }

            if (fileHeader.transfer_dimension > (uint64_t) 0L) fileHeader.transfer_dimension -= reciv;
            processi[slot].transferred += reciv;
        }
        unsigned int bytesInL, bytesInH, bytesOutL, bytesOutH;
        if (zip){
#ifdef BZIP
            BZ2_bzWriteClose64(&err, zip_p, 0, &bytesInL, &bytesInH, &bytesOutL, &bytesOutH);
#endif
        }
        fclose(fp);
        if (zip) printf("ZIPPED\n");
        processi[slot].filesTransferred++;
    }

    term:
    free(r_data);
    time_t deltaTime = time(NULL) - header.time;
    printf("Backup finished on slot %d in %ld seconds.\n", slot, deltaTime);
    closeSocket(reciver);
    closeSocket(tcps);
    processi[slot].socket = -10;
    exit(0);
}

int check_host_validity(sockaddr_in * client){
    int res = 0;
    unsigned int hip = (client)->sin_addr.s_addr;

    for (int i = 0; i < configs.ipsNo; i++)
        if (hip == managers[i].ip) {
            return 2;
        }


    for (int i = 0; i < configs.ipsNo; i++)
        if (hip == allowedHosts[i].ip) {
            res = 1;
            break;
        }

    if (res && configs.allowEqualsDeny_ips) return 0;

    if (!res) {
        for (int i = 0; i < configs.netsNo; i++)
            if ((hip & allowedNetworks[i].netMask) ==
                (allowedNetworks[i].address.ip & allowedNetworks[i].netMask)) {
                res = 1;
                break;
            }

        res ^= configs.allowEqualsDeny_nets;
    }
    return res;
}

int main(void) {
    signal(SIGINT, intHandler);
    signal(SIGHUP, intHandler);

    struct sigaction sigchld_action;
    sigchld_action.sa_handler = SIG_DFL;
    sigchld_action.sa_flags = SA_NOCLDWAIT;

    sigaction(SIGCHLD, &sigchld_action, NULL);

    pid_t serverpid = getpid();

    if (loadCfg() == -1) return 42;

    processi = (backupThread *) mmap(NULL, configs.port_interval * sizeof(backupThread), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    for (int i = 0; i < configs.port_interval; i++) {
        processi[i].port = i + configs.starting_port;
        processi[i].socket = -10;
        processi[i].status = 0;
    }

    pid_t pid = fork();
    if (!(pid < 0 || pid != 0)){
        init_terminal(serverpid, processi, &configs);
        exit(0);
    }

#ifdef SSH
    pid_t pidssh = fork();
    if (!(pidssh < 0 || pidssh != 0)){
        if(usessh) for(;;) serve_ssh_terminal(serverpid, processi, &configs, sshconfigs);
        exit(0);
    }
#endif
#ifdef GMP
    loadValues(rsacfg);
#endif

    server = newUDPSocket_server(configs.server_port);

    do {

        udpPacketRecv_t ricevuto;
        sockaddr client;
        socklen_t len = (socklen_t) sizeof(client);

        recvfrom(server, &ricevuto, sizeof(ricevuto) - 1, 0, &client, &len);
        int res = check_host_validity((sockaddr_in *) &client);

        switch ((ricevuto.mw[0] + ricevuto.mw[1]) * (res > 0 ? 1 : 0)) {
            case 0:
                printf("Connection refused\n");
                break;

            case ('m' + 'o') * 1:

                if(res != 2) break;

                /*pid_t pid;
                pid = fork();
                if (pid < 0 || pid != 0) break;*/

                char cmd[4];
                for(int i = 0; i < 3; i++) cmd[i] = ricevuto.data[i];
                cmd[3] = '\0';

                parseCmd(cmd, ricevuto.data, server, &client, len, &configs, processi);

                break;


            case ('i' + 'd') * 1:
                if (keepRunning) sendIdentification(server, client, len, ricevuto);
                break;

            case ('p' + 'k') * 1:
                if (keepRunning) sendPkey(server, client, len);
                break;

            case ('b' + 'a') * 1:
                if (keepRunning) newBackup(server, client, len, ricevuto);
                break;

            default:
                break;

        }
        fflush(stdout);

    } while (keepRunning);

    kill(pid, SIGKILL);

#ifdef SSH
    kill(pidssh, SIGKILL);
#endif

    munmap(processi, configs.port_interval * sizeof(backupThread));
    closeSocket(server);

    return 0;
}
