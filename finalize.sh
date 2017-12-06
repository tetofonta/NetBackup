#!/usr/bin/env bash

echo "Creazione scheletro directory"

rm -rf Compiled 2> /dev/null
mkdir Compiled
mkdir Compiled/Server
mkdir Compiled/Server/BACKUPS
mkdir Compiled/Server/ssh_keys
mkdir Compiled/Client
mkdir Compiled/Client/archive
mkdir Compiled/Utils

echo "Copiatura configurazioni"

cp ../src/client.properties Compiled/Client/client.properties
cp ../src/config.properties Compiled/Server/config.properties
cp ../src/monitor.py Compiled/Server/monitor.py
cp -R ../src/py_backend Compiled/Server/py_backend
cp -R ../src/remi Compiled/Server/remi
cp -R ../src/comandi Compiled/Server/comandi

echo "Generazione certificati"

openssl genpkey -algorithm RSA -out private_key.pem -pkeyopt rsa_keygen_bits:1024
python3 ../src/genRSACFG.py private_key.pem Compiled/Server/keys.rsacfg
rm -rf private_key.pem
openssl req -new -x509 -keyout Compiled/Server/server.pem -out server.pem -days 365 -nodes -subj"/C=/ST=/L=/O=/OU=/CN="
ssh-keygen -t dsa -f Compiled/Server/ssh_keys/ssh_host_dsa_key -N ''
ssh-keygen -t rsa -f Compiled/Server/ssh_keys/ssh_host_rsa_key -N ''
openssl genpkey -algorithm RSA -out CA.pem -pkeyopt rsa_keygen_bits:2048
python3 ../src/genRSACFG.py CA.pem Compiled/Utils/CA.rsacfg

echo "Copiatura eseguibili"

cp ./src/client Compiled/Client/client
cp ./src/server Compiled/Client/server
cp ./src/baknfo Compiled/Utils/baknfo
cp ./src/extract Compiled/Utils/extract
cp ./src/extract_all Compiled/Utils/extract_all
cp ./src/genPasswd Compiled/Utils/genPasswd
cp ./src/signElf Compiled/Utils/signElf