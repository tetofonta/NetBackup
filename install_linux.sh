#!/bin/bash

USRID=`id -u`

if [ $USRID -ne "0" ]
	then 
	echo "Nessun privilegio di root rilevato."
	exit
fi

read -p "Install directory: " HOME

echo "Building executables ###############################################################"
sh linux_build.sh

if ! [ -z $1 ]
then
	if [ $1 = "test" ]
	then
		echo "Testing ###############################################################"
		cd ./build/Test/test/
		ctest
		cd ../../../
	fi
fi

echo "Generating output packets ###############################################################"

read -p "Vuoi compilere il codice lato client? [y/N]: " YN

if [ $YN = "y" ] || [ $YN = "Y"]
then
	mkdir ./output
	mkdir ./output/archive
	cp ./src/client.properties ./output/client.properties
	cp ./build/src/client ./output/netbackup_client
fi

read -p "Vuoi compilere il codice lato server? [y/N]: " YN

if [ $YN = "y" ] || [ $YN = "Y"]
then
	mkdir "$HOME/networkBackup"
	mkdir "$HOME/networkBackup/ssh_keys"
	mkdir "$HOME/networkBackup/BACKUPS"
	cp ./src/config.properties "$HOME/networkBackup/config.properties"
	cp ./build/src/server "$HOME/networkBackup/server"
	openssl genpkey -algorithm RSA -out private_key.pem -pkeyopt rsa_keygen_bits:1024
	python3 ./src/genRSACFG.py private_key.pem "$HOME/networkBackup/keys.rsacfg"
	ssh-keygen -t dsa -f "$HOME/networkBackup/ssh_keys/ssh_host_dsa_key" -N ''
	ssh-keygen -t rsa -f "$HOME/networkBackup/ssh_keys/ssh_host_rsa_key" -N ''
	openssl genpkey -algorithm RSA -out CA.pem -pkeyopt rsa_keygen_bits:2048
	python3 ./src/genRSACFG.py CA.pem ./CA.rsacfg
	cp ./CA.rsacfg.pub "$HOME/networkBackup/CA.public"

	read -p "Vuoi registrare il servizio server? [y/N]: " YN
	if [ $YN = "y" ] || [ $YN = "Y"]
	then
		echo "[Unit]\nDescription=NetBackup server - Fontana Stefano (tetofonta)\nAfter=network.target\n[Service]\nUser=root\nWorkingDirectory=$HOME/networkBackup\nType=oneshotmc\nRemainAfterExit=yes\nExecStart=/bin/sh $HOME/networkBackup/server_start.sh\nExecStop=/bin/sh $HOME/networkBackup/server_start.sh\n[Install]\nWantedBy=multi-user.target" > netback_server.service
		cp server_start.sh "$HOME/networkBackup/server_start.sh"
		cp server_stop.sh "$HOME/networkBackup/server_stop.sh"
		#cp ./netback_server.service /etc/systemd/system/netback_server.service
	fi

	read -p "Vuoi configurare il server di monitoraggio? [y/N]: " YN
	if [ $YN = "y" ] || [ $YN = "Y"]
	then
		wget http://download.oracle.com/glassfish/5.0/release/glassfish-5.0.zip
		unzip glassfish-5.0.zip -d "$HOME"
		cp ./monitor.war "$HOME/monitor.war"
	fi
fi

read -p "Vuoi compilere le utilities? [y/N]: " YN

if [ $YN = "y" ] || [ $YN = "Y"]
then
	mkdir ./utils
	cp ./build/src/baknfo ./utils/baknfo
	cp ./build/src/extract ./utils/extract
	cp ./build/src/extract_all ./utils/extract_all
	cp ./build/src/genPasswd ./utils/genPasswd
	cp ./build/src/signElf ./utils/signElf
fi

