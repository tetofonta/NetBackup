project(NetBak)

add_library(_FS FileSystem/FS.c)
add_library(_AES AES/aes.c)
add_library(_B32 BASE_encoding/base32.c)
add_library(_CFG config/config.c)
add_library(_MD5 MD5/md5.c)
add_library(_NET Networking/tcp.c Networking/udp.c)
add_library(_PACKETS packets.c)
add_library(_RSA RSA/IO.cpp RSA/utilities.cpp RSA/Signature.cpp)
add_library(_TERM Terminal/terminal.cpp Terminal/sshSession.cpp)
target_compile_definitions(_TERM PUBLIC GMP)
target_link_libraries(_TERM _MD5 dl gmp)

#to find gmp, bzip2 ssh
add_executable(client client.cpp)
target_compile_definitions(client PUBLIC GMP)
target_link_libraries(client _FS _AES _B32 _CFG _NET _PACKETS _RSA gmp)

if (UNIX)
    add_executable(server server.cpp commands.cpp)
    target_compile_definitions(server PUBLIC GMP)
    target_compile_definitions(server PUBLIC BZIP)
    target_compile_definitions(server PUBLIC SSH)
    target_link_libraries(server _CFG _NET _RSA _PACKETS _TERM gmp bz2 ssh)
endif (UNIX)

add_executable(baknfo bakInfo.cpp)

add_executable(extract extract.cpp)
target_link_libraries(extract _AES)

add_executable(extract_all extract_all.cpp)
target_link_libraries(extract_all _AES _FS)

add_executable(genPasswd genPassword.cpp)
target_link_libraries(genPasswd _B32 _MD5)

add_executable(signElf signElf.cpp)
target_compile_definitions(signElf PUBLIC GMP)
target_link_libraries(signElf _RSA _MD5 gmp)