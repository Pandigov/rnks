#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <io.h>
#include <C:\Users\Pandigo\Desktop\BelegRNks\rnks\rnks\checksum.h>
#include <C:\Users\Pandigo\Desktop\BelegRNks\rnks\rnks\struct.h>
#include <ctype.h>
#pragma warning(disable:4996)
#pragma comment(lib, "Ws2_32.lib")

#define MAX_DATA_SIZE 100
#define WINDOW_SIZE 1

SOCKET sockfd;
unsigned short portnr = 2000;
struct sockaddr_in6 addr;

int initserver() {
    
    struct addrinfo* result = NULL,
                   * pointer = NULL,
                     hints;
    unsigned short portnr = 2000;
    struct sockaddr_in6 addr;
    WSADATA wsa;

    int ret = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (ret != 0) {
        printf("WSAStartup() failed: %d\n", ret);
        return 1;
    }

    /* Open output file
    FILE* fp = fopen("received.txt", "w");
    if (fp == NULL) {
        perror("fopens");
        return -1;
    }
    */
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;

    int retaddr = getaddrinfo(NULL, "5001", &hints, &result);
    if (retaddr != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(retaddr));
        exit(EXIT_FAILURE);
    }

    for (pointer = result; pointer != NULL; pointer = pointer->ai_next) {
        sockfd = socket(pointer->ai_family, pointer->ai_socktype, pointer->ai_protocol);
        if (sockfd == -1) {
            continue;
        }

        if (bind(sockfd, pointer->ai_addr, pointer->ai_addrlen) == 0) {
            break;
        }
    }
    
    freeaddrinfo(result);
    if (pointer == NULL) {
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }
   // addr.sin6_family = AF_INET6;
    //addr.sin6_port = htons(5000);
   // addr.sin6_addr = in6addr_any;

    // Binde den Socket an die Adresse
   // bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
   
}

int getReq() {

    // Initialize variables
    int recv_bytes;
    static long expected_seqnum = 0;
    socklen_t addr_len = sizeof(struct sockaddr_in6);
    struct request req;


    // receive message from socket
    recv_bytes = recvfrom(sockfd, (char*)&req, sizeof(req), 0, (struct sockaddr*)&addr, &addr_len);
    if (recv_bytes == -1) {
        perror("recvfrom");
        exit(-1);
    }

    if (recv_bytes == 0) {
        printf("Client closed connection\n");
        closesocket(sockfd);
        exit(-1);
    }

    return &req;
}

int sendAck(struct answer* answ) {

    int senAck=sendto(sockfd, (const char*)answ, sizeof(*answ), 0, (struct sockaddr*)&addr, sizeof(addr));

    if (senAck == -1) {
        perror("sendto error", WSAGetLastError());
        return -1;
    }
    printf("\t Ack gesendet: SeqNr: %d", answ->seqnr);

    
}
