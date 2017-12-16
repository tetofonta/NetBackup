[![Build Status](https://travis-ci.org/tetofonta/NetBackup.svg?branch=master)](https://travis-ci.org/tetofonta/NetBackup)

![LOGO](/images/backup.png)
# NETBACK
#### Software di trasferimento e salvataggio file attraverso la rete.

## Presentazione del software e funzionalità

Il software viene distribuito come suite di programmi e funzionalità atti alla trasmissione sicura di file attraverso la rete.
Gli applicativi vengono compilati e la loro esecuzione avviene in ambiente console, oltre a una distribuzione web basata su JavaEE/Glassfish per il monitoraggio e la gestione dei flussi di backup.
Nelle configurazioni è possibile abilitare la trasmissione cifrata mediante algoritmo AES a 256bit, oltre all’abilitazione dell’ottimizzazione della memoria usata: se specificata il trasferimento verrà compresso al momento del salvataggio sul disco mediante algoritmo bzip2.
Viene implementato un terminale con l’esecuzione di comandi HotPlug, grazie al caricamento di librerie dinamiche.

## Applicativi Compilati
### Funzionalità
####Controllo dell’identità del server mediante RSA
Il primo controllo del client nei confronti del server consiste nella verifica dell’identità della macchina mediante la firma digitale, possibile grazie all’algoritmo RSA.
Al momento della configurazione del server verrà creata una coppia di chiavi (Pubblica e Privata) contenente le informazioni relative ai valori per compiere i passaggi di firmatura: questi dati sono contenuti nel file “keys.rsacfg” (Configurazione di default).

![PROCESSO DI FIRMA](/images/signing.png)

Come Visibile dall’immagine, se il client possiede già una copia della chiave pubblica del server, procederà all’invio di una stringa di 16 caratteri che verrà firmata digitalmente dal server, per poi essere rispedita al mittente e verificata.
Se la verifica dovesse avere esito negativo, il server sarebbe giudicato non attendibile e di conseguenza la connessione non assicura la riservatezza dei dati.
Questo previene attacchi del tipo “Man In The Middle”.
