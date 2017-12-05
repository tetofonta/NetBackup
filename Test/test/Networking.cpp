//
// Created by stefano on 05/12/17.
//

#include "gtest/gtest.h"
#include <pthread.h>
#include <thread_db.h>
#include <Networking/Networking.h>

TEST(Networking, udpServer){
    int udp = newUDPSocket_server(5577);
    ASSERT_LT(0, udp);
    ASSERT_EQ(0, closeSocket(udp));
}

void * udpServer(void * ptr){
    int udp = newUDPSocket_server(11001);
    EXPECT_LT(0, udp);

    sockaddr client;
    socklen_t len = (socklen_t) sizeof(client);
    char ricevuto[100];
    EXPECT_LT(0, recvfrom(udp, ricevuto, 100, 0, &client, &len));
    EXPECT_STREQ(ricevuto, "stefanoFontana");

    EXPECT_EQ(0, closeSocket(udp));
    return NULL;
}

void * udpClient(void * ptr){
    connection_t udp = newUDPSocket_client(11001, "127.0.0.1", 10);
    EXPECT_LT(0, udp.socket);
    char packet[] = "stefanoFontana";
    EXPECT_LT(0, sendto(udp.socket, packet, 15, 0, (const sockaddr *) &udp.clientData, udp.clientDataLenght));
    return NULL;
}

TEST(Networking, udpComunication){
    thread_t server;
    ASSERT_EQ(0, pthread_create(&server, NULL, udpServer, NULL));

    thread_t client;
    ASSERT_EQ(0, pthread_create(&client, NULL, udpClient, NULL));

    ASSERT_EQ(0, pthread_join(server, NULL));
    ASSERT_EQ(0, pthread_join(client, NULL));
}

TEST(Networking, tcpServer){
    int tcp = newTCPSocket_server(5577);
    ASSERT_LT(0, tcp);
    ASSERT_EQ(0, closeSocket(tcp));
}

void * tcpServer(void * ptr){
    int tcp = newTCPSocket_server(11001);
    EXPECT_LT(0, tcp);

    sockaddr client;
    socklen_t len = (socklen_t) sizeof(client);
    char ricevuto[100];

    int conn = wait4conn(tcp, 5, &client, &len);
    EXPECT_LT(0, conn);

    EXPECT_LT(0, recv(conn, ricevuto, 100, 0));
    EXPECT_STREQ(ricevuto, "stefanoFontana");

    EXPECT_EQ(0, closeSocket(conn));
    EXPECT_EQ(0, closeSocket(tcp));
    return NULL;
}

void * tcpClient(void * ptr){
    int tcp = newTCPSocket_client(11001, "127.0.0.1");
    EXPECT_LT(0, tcp);
    char packet[] = "stefanoFontana";
    EXPECT_LT(0, send(tcp, packet, 15, 0));
    return NULL;
}

TEST(Networking, tcpComunication){
    thread_t server;
    ASSERT_EQ(0, pthread_create(&server, NULL, tcpServer, NULL));

    thread_t client;
    ASSERT_EQ(0, pthread_create(&client, NULL, tcpClient, NULL));

    ASSERT_EQ(0, pthread_join(server, NULL));
    ASSERT_EQ(0, pthread_join(client, NULL));
}