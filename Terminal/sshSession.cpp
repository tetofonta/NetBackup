#include <stdlib.h>
#include "terminal.h"

#ifdef SSH
    #include <libssh/libssh.h>
    #include <libssh/server.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <signal.h>
#include "terminal.h"
#include "../MD5/md5.h"

char buf[2048];
char foo[2048];
ssh_session session;
ssh_bind sshbind;
ssh_message message;
ssh_channel gchan = 0;
int auth = 0;
int sftp = 0;
int i;
int r;


ssh_pcap_file pcap;

void set_pcap(ssh_session session, const char *pcap_file) {
    if (!pcap_file)
        return;
    pcap = ssh_pcap_file_new();
    if (ssh_pcap_file_open(pcap, pcap_file) == SSH_ERROR) {
        printf("Error opening pcap file\n");
        ssh_pcap_file_free(pcap);
        pcap = NULL;
        return;
    }
    ssh_set_pcap_file(session, pcap);
}

void cleanup_pcap(void);

void cleanup_pcap() {
    ssh_pcap_file_free(pcap);
    pcap = NULL;
}

int printToSSH(const char * pattern, ...){
    bzero(foo, 2048);
    va_list argptr;
    va_start(argptr, pattern);
    int printed = vsprintf(foo, pattern, argptr);
    va_end(argptr);
    ssh_channel_write(gchan, foo, printed);
    return printed;
}

static int auth_password(const char *user, const char *password, sshconf *cfg) {
    if (strcmp(user, cfg->user)) return 0;

    char *ciao = (char *) malloc(16);
    char md5hash[33];
    md5((uint8_t *) password, strlen(password), ciao);

    for (int i = 0; i < 16; i++) sprintf(md5hash + 2 * i, "%2.2x", ciao[i] & 0xFF);

    if (strcmp(md5hash, cfg->password)) return 0;

    return 1;
}

int handle_session(ssh_bind sshbind, ssh_session session, pid_t serverpid, backupThread *baks, conf * confs, sshconf cfg) {
    ssh_channel chan = 0;
    ssh_message message;
    char buf[2048];
    int auth = 0;
    int sftp = 0;

    if (ssh_handle_key_exchange(session)) {
        printf("ssh_handle_key_exchange: %s\n", ssh_get_error(session));
        return 1;
    }
    do {
        message = ssh_message_get(session);
        if(!message)
            break;
        switch(ssh_message_type(message)){
            case SSH_REQUEST_AUTH:
                switch(ssh_message_subtype(message)){
                    case SSH_AUTH_METHOD_PASSWORD:
                            if (auth_password(ssh_message_auth_user(message), ssh_message_auth_password(message), &cfg)) {
                            auth=1;
                            ssh_message_auth_reply_success(message,0);
                            break;
                        }
                        // not authenticated, send default message
                    case SSH_AUTH_METHOD_NONE:
                    default:
                        ssh_message_auth_set_methods(message,SSH_AUTH_METHOD_PASSWORD);
                        ssh_message_reply_default(message);
                        break;
                }
                break;
            default:
                ssh_message_reply_default(message);
        }
        ssh_message_free(message);
    } while (!auth);
    if(!auth){
        printf("auth error: %s\n",ssh_get_error(session));
        ssh_disconnect(session);
        return 1;
    }
    do {
        message = ssh_message_get(session);
        if(message){
            switch(ssh_message_type(message)){
                case SSH_REQUEST_CHANNEL_OPEN:
                    if(ssh_message_subtype(message)==SSH_CHANNEL_SESSION){
                        chan=ssh_message_channel_request_open_reply_accept(message);
                        break;
                    }
                default:
                    ssh_message_reply_default(message);
            }
            ssh_message_free(message);
        }
    } while(message && !chan);
    if(!chan){
        printf("error : %s\n",ssh_get_error(session));
        ssh_finalize();
        return 1;
    }
    do {
        message=ssh_message_get(session);
        if(message && ssh_message_type(message)==SSH_REQUEST_CHANNEL &&
           ssh_message_subtype(message)==SSH_CHANNEL_REQUEST_SHELL){
//            if(!strcmp(ssh_message_channel_request_subsystem(message),"sftp")){
            sftp=1;
            ssh_message_channel_request_reply_success(message);
            break;
            //           }
        }
        if(!sftp){
            ssh_message_reply_default(message);
        }
        ssh_message_free(message);
    } while (message && !sftp);
    if(!sftp){
        printf("error : %s\n",ssh_get_error(session));
        return 1;
    }
    printf("it works !\n");
    int i;

    ssh_channel_write(chan, "Backup server>> ", 16);
    do {
        bzero(buf, 2048);
        i = ssh_channel_read(chan, buf, 2048, 0);
        if (i > 0) {
            strtok(buf, "\n");
            ssh_channel_write(chan, "Backup server>> ", 16);
            if(strcmp(buf, "exit")){
                gchan = chan;
                parseCommand(buf, strlen(buf), baks, confs, printToSSH);
            }
        }
    } while (i > 0 && strcmp(buf, "exit"));

    if(!strcmp(buf, "exit")) kill(serverpid, SIGHUP);

    ssh_disconnect(session);
    ssh_bind_free(sshbind);
    ssh_finalize();
    return 0;
}

int serve_ssh_terminal(pid_t serverpid, backupThread *baks, conf * confs, sshconf cfg){
    sshbind = ssh_bind_new();

    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_DSAKEY, cfg.dsa);
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_RSAKEY, cfg.rsa);
    ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT, &(cfg.port));


    if (cfg.usepcap) set_pcap(session, cfg.pcap);

    if(ssh_bind_listen(sshbind)<0){
        printf("Error listening to socket: %s\n",ssh_get_error(sshbind));
        return 1;
    }

    while (1) {
        ssh_session session = ssh_new();
        int r = ssh_bind_accept(sshbind, session);
        if(r == SSH_ERROR){
            fprintf(stderr, "error accepting a connection : %s\n",ssh_get_error(sshbind));
            return 1;
        }
        else if (!fork()) {
            handle_session(sshbind, session, serverpid, baks, confs, cfg);
            fprintf(stderr, "session terminated\n");
            exit(0);
        }
    }
}

#else

int serve_ssh_terminal(pid_t serverpid, backupThread * baks, conf * confs, sshconf cfg){
    return 0;
}

#endif