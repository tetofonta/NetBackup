
slibraries = -lgmp -lbz2 -lssh
clibraries = -lgmp
CDEF = -DGMP
SDEF = -DGMP -DBZIP -DSSH

all: prepare compile

check:
	gcc Tests/AES_test.c AES/aes.c -o AES_TEST
	./AES_TEST

prepare: makeDirs copyAll generate makeComandi

makeDirs:
	rm -rf Compiled 2> /dev/null
	mkdir Compiled
	mkdir Compiled/Server
	mkdir Compiled/Server/BACKUPS
	mkdir Compiled/Server/ssh_keys
	mkdir Compiled/Client
	mkdir Compiled/Client/archive
	mkdir Compiled/Utils

copyAll:
	cp client.properties Compiled/Client/client.properties
	cp config.properties Compiled/Server/config.properties
	cp monitor.py Compiled/Server/monitor.py
	cp -R py_backend Compiled/Server/py_backend
	cp -R remi Compiled/Server/remi
	cp -R comandi Compiled/Server/comandi

generate:
	openssl genpkey -algorithm RSA -out private_key.pem -pkeyopt rsa_keygen_bits:1024
	python3 genRSACFG.py private_key.pem Compiled/Server/keys.rsacfg
	rm -rf private_key.pem
	openssl req -new -x509 -keyout Compiled/Server/server.pem -out server.pem -days 365 -nodes
	ssh-keygen -t dsa -f Compiled/Server/ssh_keys/ssh_host_dsa_key -N ''
	ssh-keygen -t rsa -f Compiled/Server/ssh_keys/ssh_host_rsa_key -N ''
	openssl genpkey -algorithm RSA -out CA.pem -pkeyopt rsa_keygen_bits:2048
	python3 genRSACFG.py CA.pem CA.rsacfg
	mv CA.rsacfg Compiled/Utils/CA.rsacfg
	mv CA.rsacfg.pub Compiled/Server/CA.public

makeComandi:
	echo "DA IMPLEMENTARE"

compile:
	g++ client.cpp BASE_encoding/base32.c FileSystem/FS.c packets.c Networking/tcp.c RSA/Signature.cpp RSA/IO.cpp RSA/utilities.cpp AES/aes.c Networking/udp.c config/config.c -o Compiled/Client/network_backup_client -Ofast $(clibraries) $(CDEF)
	g++ server.cpp Terminal/terminal.c Terminal/sshSession.cpp commands.cpp packets.c Networking/tcp.c AES/aes.c Networking/udp.c RSA/Signature.cpp RSA/IO.cpp RSA/utilities.cpp config/config.c MD5/md5.c -o Compiled/Server/server -Ofast -ldl $(slibraries) $(SDEF)
	g++ extract.cpp AES/aes.c -o Compiled/Utils/extract -Ofast $(slibraries) $(SDEF)
	g++ genPassword.cpp MD5/md5.c BASE_encoding/base32.c -o Compiled/Utils/genPasswd -Ofast
	g++ signElf.cpp RSA/Signature.cpp RSA/IO.cpp RSA/utilities.cpp MD5/md5.c -o Compiled/Utils/signElf $(slibraries) $(SDEF)
