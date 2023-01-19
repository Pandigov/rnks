
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <io.h>
#include <C:\Users\Pandigo\Desktop\BelegRNks\rnks\rnks\struct.h>
#include <C:\Users\Pandigo\Desktop\BelegRNks\rnks\rnks\checksum.h>
#pragma warning(disable:4996)

#define WINDOW_SIZE 1



struct answer* processRequest(struct request* req) {

	unsigned int recChecksum = req->checksum;
	int expected_seq_num = 0;
	static struct answer answ;
	ZeroMemory(&answ, sizeof(answ));

	unsigned int calcChecksum = checksum((unsigned short*)req, sizeof(struct request));

	if (calcChecksum != recChecksum)
	{
		printf("\tFalsche Checksumme: Erhalten: %d  Berechnet: %d", recChecksum, calcChecksum);
	}

	printf("\t Erhaltene Sequenznummer: %d\n", req->seqnr);

	if (req->seqnr != expected_seq_num)
	{
		printf("Unexpected sequence number received: %d, expected: %d\n", req->seqnr, expected_seq_num);
	}
	else {
		expected_seq_num = expected_seq_num + req->datalen;

	}

}

int main(int argc, char const* argv[]) {

	struct answer* answ;
	// ZeroMemory(&answ, sizeof(answ));
	struct request* req;
	int recv_bytes;

	printf("Server wartet...");
	initserver();

	while (1)
	{
		req = getReq();
		if (req != NULL)
		{
			printf("datalen: %ld\n", req->datalen);
			printf("seqnr: %lu\n", req->seqnr);
			printf("data: %s\n", req->data);
			printf("checksum: %hu\n", req->checksum);
			answ = processRequest(req);
			if (answ != NULL) {
				sendAck(answ);
			}
			else {
				printf("\tKein ACKNOWLEDGEMENT gesendet\n");
			}
		}

	}

}

