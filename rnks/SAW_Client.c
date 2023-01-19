
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _WINSOCKAPI_ 
#include <winsock2.h>
#include <ws2tcpip.h>
#include <io.h>
#include <C:\Users\Pandigo\Desktop\BelegRNks\rnks\rnks\checksum.h>
#include <C:\Users\Pandigo\Desktop\BelegRNks\rnks\rnks\struct.h>
#pragma warning(disable:4996)
#pragma comment(lib, "Ws2_32.lib")


void setnonblocking(SOCKET sockfd) {
	int iMode = 1;
	int iRes=ioctlsocket(sockfd, FIONBIO, &iMode);
}

SOCKET sockfd;
char* server_port = "50000";
unsigned short portnr = 2000;
static struct sockaddr_in6 server_addr;
char* ip = "127.0.0.1";

int initclient(char* server)
{
	struct addrinfo* result = NULL,
		* pointer = NULL,
		hints;
	static struct addrinfo server_addr;

	WSADATA wsa;

	int ret = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (ret != 0) {
		printf("WSAStartup() failed: %d\n", ret);
		return 1;
	}
	
	
	// Create socket
	sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

	if (sockfd == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	//char buf[MAX_BUF_LEN] = "Hallo Client!";

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	

	if (server==NULL)
	{
		hints.ai_flags = AI_PASSIVE;
		server = "::1";
	}
	printf("Addresse: %s ", server);

	int retaddr = getaddrinfo(NULL, portnr, &hints, &result);
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

	struct packet {
		unsigned int seqnum;
		char data[500];
	};
	
	// set nonblocking
	setnonblocking(sockfd);
}

struct answer *doReq(struct request* req) {
	int Timeout = 0;
	int sentlen;
	unsigned int seqnum = 0;
	int sel_res = 0;
	fd_set rfds;
	struct answer answ;



	while (Timeout)
	{
		//sentlen = sendto(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr*)dest_addr, sizeof(*dest_addr));
		sentlen = sendto(sockfd, (char*)req, sizeof(req), 0, (struct sockaddr*)ip, sizeof(server_addr));
		if (sentlen == -1) {
			perror("sendto error", WSAGetLastError());
			return -1;
		}
		printf("\tpaket gesendet\tSequenznummer: %d", req->seqnr);
		//int reclen = recvfrom(sockfd, buf, MAX_BUF_LEN, 0, 0, 0);
		//printf("Empfangene Nachricht vom Client: %s\n", buf);

		struct timeval timeout;
		timeout.tv_sec = 15;
		timeout.tv_usec = 0;



		while (1) {
			// Clear file descriptor set
			FD_ZERO(&rfds);

			// Add socket to file descriptor set
			FD_SET(sockfd, &rfds);

			// Wait for data to be available on socket
			sel_res = select(sockfd + 1, &rfds, NULL, NULL, &timeout);

			// Check for error
			if (sel_res < 0) {
				perror("select");
				exit(EXIT_FAILURE);
			}

			// Check for timeout
			if (sel_res == 0) {
				printf("Timeout reached\n");
				break;
			}

			// Data is available on socket, receive it
			int reclen = recvfrom(sockfd, (char*)&answ, sizeof(answ), 0, NULL, NULL);
			if (reclen == SOCKET_ERROR) {
				fprintf(stderr, "recvfrom() failed: error %d\n", WSAGetLastError());
				WSACleanup();
				exit(-1);
			}
			printf("\n\tErwartete Sequenznummer: %d\n", (req->seqnr + req->datalen));
			printf("\n\tpaket erhalten\n");

			// Check received data
			if (answ.seqnr == req->seqnr) {
				// Received expected sequence number, break loop
				break;
			}
			else {
				// Received unexpected sequence number, continue loop
				continue;
			}
	}

	WSACleanup();
	close(sockfd);
	
}
}