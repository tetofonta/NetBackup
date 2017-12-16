[![Build Status](https://travis-ci.org/tetofonta/NetBackup.svg?branch=master)](https://travis-ci.org/tetofonta/NetBackup)

![LOGO |100x100,20%](/images/backup.png)
# NETBACK
#### Software di trasferimento e salvataggio file attraverso la rete.

## Presentazione del software e funzionalità

Il software viene distribuito come suite di programmi e funzionalità atti alla trasmissione sicura di file attraverso la rete.
Gli applicativi vengono compilati e la loro esecuzione avviene in ambiente console, oltre a una distribuzione web basata su JavaEE/Glassfish per il monitoraggio e la gestione dei flussi di backup.
Nelle configurazioni è possibile abilitare la trasmissione cifrata mediante algoritmo AES a 256bit, oltre all’abilitazione dell’ottimizzazione della memoria usata: se specificata il trasferimento verrà compresso al momento del salvataggio sul disco mediante algoritmo bzip2.
Viene implementato un terminale con l’esecuzione di comandi HotPlug, grazie al caricamento di librerie dinamiche.

## Applicativi Compilati
### Funzionalità
#### Controllo dell’identità del server mediante RSA
Il primo controllo del client nei confronti del server consiste nella verifica dell’identità della macchina mediante la firma digitale, possibile grazie all’algoritmo RSA.
Al momento della configurazione del server verrà creata una coppia di chiavi (Pubblica e Privata) contenente le informazioni relative ai valori per compiere i passaggi di firmatura: questi dati sono contenuti nel file “keys.rsacfg” (Configurazione di default).

![PROCESSO DI FIRMA](/images/signing.png)

Come Visibile dall’immagine, se il client possiede già una copia della chiave pubblica del server, procederà all’invio di una stringa di 16 caratteri che verrà firmata digitalmente dal server, per poi essere rispedita al mittente e verificata.
Se la verifica dovesse avere esito negativo, il server sarebbe giudicato non attendibile e di conseguenza la connessione non assicura la riservatezza dei dati.
Questo previene attacchi del tipo “Man In The Middle”.

#### Crittografia AES ECB a 256 bit
Se definito nel file di configurazione del client, il file trasmesso sarà cifrato mediante AES 256.
La crittografie prende parte all’invio del file a blocchi di lunghezza configurabile ma multiplo di 16, dimensione del blocco AES.
Il server si limiterà a salvare il file così come viene inviato, e, mediante un eseguibile di utilità, i file potranno essere decifrati e salvati correttamente.

#### Integrità della trasmissione
La connessione TCP implementa nativamente la ricezione del pacchetto con risposta (ACK) e verifica di integrità dei dati tramite CRC32
Gestione dello spazio su disco
All’atto della ricezione del pacchetto, il server, salverà sul disco fisso i dati, e se specificato nel file di configurazione, il contenuto del pacchetto verrà compresso con l’algoritmo BZip2, mantenendo l’occupazione in memoria Ram limitata fissando un limite massimo di dimensione del file prima di essere zippato e scaricato sul disco.

#### Terminale di gestione interno
Un processo adibito alle funzioni di terminale filtra lo stream STDIN ed esegue i comandi caricandoli al momento come librerie dinamiche.
Prima di caricare il codice, per prevenire l’esecuzione di codice malevolo, il file viene verificato mediante il processo di firma digitale utilizzato per la verifica dell’identità del server.
In questo caso una CA (Certificate Authority) viene generata (set di chiavi) al momento della compilazione, per ragioni di sicurezza, la chiave privata e pubblica non dovranno mai esistere sullo stesso hard disk.
![Terminale](/images/terminal.png)
#### Shell SSH
Se impostato al momento della creazione, il server creerà un processo per la gestione delle connessioni SSHv1 e SSHv2 sulle quali verranno reindirizzati gli stream di I/O del terminale.
Questo soltanto se al momento della creazione riesce la creazioni di chiavi RSA e DSA atte alla gestione della shell sicura.

### Utilità
All’atto della compilazione verranno generati tre eseguibili e un file di configurazione rsa che, bensì situati in altre directory, per motivi di sicurezza è fortemente consigliata la separazione fisica degli eseguibili su diverse macchine.
È di conseguenza consigliata la cancellazione degli eseguibili di utilità dal server e mantenerli su una seconda macchina possibilmente non raggiungibile dalla rete del server, in quanto il file di configurazione rsa (CA.rsacfg) contiene la chiave privata per la segnatura dei comandi.
#### ./genPasswd - Codificatore di password
```
SINTASSI: ./genPasswd <password>
OUTPUT: l’hash generato relativo al parametro <password> inserito e la chiave codificata.
```

![Passwd](/images/genpasswd.png)

La sessione ssh verificherà l’autenticazione di un solo utente dati il suo nome utente e l’hash della password.
Al momento della convalida delle credenziali, il server verificherà le coincidenze del nome utente, l’hash salvato e fornito al momento della configurazione e l’hash della password inserita dall’utente generato runtime.
Al momento della configurazione, dopo aver generato il corrispondente hash sarà necessario trascrivere l’output del programma nel file di configurazione.


#### ./signElf - Firma digitale dei comandi

```
SINTASSI: ./signElf <percorso/al/comando>
OUTPUT: none
```
![signelf](/images/signelf.png)

Per verificare l’attendibilità prima dell’esecuzione di un programma il server verificherà la firma digitale del file generata da signElf.
La firma verrà aggiunta in coda all’eseguibile e consisterà nella rappresentazione esadecimale dell’hash del file segnato digitalmente con la chiave privata dell’autorità di certificazione.
Una volta svincolato il dato dalla firma digitale mediante la chiave pubblica salvata, il risultato verrà confrontato con l’hash del file (Firma esclusa).
Se gli hash variassero il comando non verrebbe eseguito perché modificato.

#### ./baknfo - Informazioni di backup
Il tool stamperà a schermo le informazioni relative al file head.hd nella directory del backup.

#### ./extract_all - Estrazione di tutti i file crittografati
```
SINTASSI: ./extract_all <directory del backup> <packet_lenght>
<key>
OUTPUT: none
```
Il funzionamento viene illustrato nel capitolo “./extract - Estrazione dei file crittografati” e viene eseguito su tutti i file con estensione “*.bak”.

#### ./extract - Estrazione dei file crittografati
```
SINTASSI: ./extract <input_file> <packet_lenght> <key> [output_file]
OUTPUT: none
```
Durante l’esecuzione, l’applicazione legge dal file <input_file> a blocchi di <packet_lenght>.
Quest’ultimo deve equivalere al valore (transfer_block_size x 16)
Una volta letto l’header del file, con definizione dei parametri quali la lunghezza del file e se quest’ultimo sia cifrato, verrà creato un file chiamato [output_file] oppure, se non definito, verrà creato un file con il nome definito nell’header di trasmissione nella directory di esecuzione dell’applicazione.
Nel caso il file sia cifrato, il salvataggio avverà in chiaro.
Nel caso in un backup fossero salvati più file con lo stesso nome, essi verranno salvati con il suffisso “-_<variante>”.

### Workflow

####Client
![CLIENT WF](/images/client.png)

#### Server