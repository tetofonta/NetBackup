#!/bin/bash

USRID=`id -u`

if [ $USRID -ne "0" ]
	then 
	echo "Nessun privilegio di root rilevato."
	exit
fi

read -p "Install directory (Absolute path): " HOME

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
		unzip glassfish-5.0.zip -d "$HOME/networkBackup"
		cp ./monitor.war "$HOME/networkBackup/monitor.war"
		wget https://dev.mysql.com/get/Downloads/Connector-J/mysql-connector-java-5.1.45.zip
		unzip mysql-connector-java-5.1.45.zip
		cp ./mysql-connector-java-5.1.45/mysql-connector-java-5.1.45-bin.jar "$HOME/networkBackup/glassfish5/glassfish/lib/mysql-connector-java-5.1.45-bin.jar"
		cp ./mysql-connector-java-5.1.45/mysql-connector-java-5.1.45-bin.jar "$HOME/networkBackup/glassfish5/glassfish/domains/domain1/lib/mysql-connector-java-5.1.45-bin.jar"
		echo "Devi configurare il tuo server come riportato nel file INSTALL\n"
		read -p "press any key to continue..." foo

		read -p "Vuoi registrare il servizio per il server di monitoraggio? [y/N]: " YN
			if [ $YN = "y" ] || [ $YN = "Y"]
			then
				echo "[Unit]\nDescription=NetBackup server monitor - Fontana Stefano (tetofonta)\nAfter=network.target\n[Service]\nUser=root\nWorkingDirectory=$HOME/networkBackup/glassfish5/bin\nType=oneshotmc\nRemainAfterExit=yes\nExecStart=$HOME/networkBackup/glassfish5/bin/asadmin start-domain\nExecStop=$HOME/networkBackup/glassfish5/bin/asadmin stop-domain\n[Install]\nWantedBy=multi-user.target" > netback_server_monitor.service
				#cp ./netback_server_monitor.service /etc/systemd/system/netback_server_monitor.service
			fi

		cd "$HOME/networkBackup/glassfish5/bin"
		./asadmin start-domain
		./asadmin deploy ../../monitor.war
		./asadmin stop-domain
		
	fi
fi
