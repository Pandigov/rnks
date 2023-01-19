
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _WINSOCKAPI_ 
#include <winsock2.h>
#include <ws2tcpip.h>
#include <io.h>
#include <C:\Users\Pandigo\Desktop\BelegRNks\rnks\rnks\struct.h>
#include <C:\Users\Pandigo\Desktop\BelegRNks\rnks\rnks\checksum.h>
#pragma warning(disable:4996)
#define BUFFERSIZE 500


int fillreq (struct request* req, FILE* file) {
    static long pos = 0;
    long fsize;

    FILE* fp = fopen("C:\\Users\\Pandigo\\Desktop\\BelegRNks\\rnks\\rnks\\test.txt", "r");
    if (fp == NULL) {
        perror("fopen");
        fclose(fp);
        return -1;
    }

    ZeroMemory(&req, sizeof(struct request));

    // Set file position
    if (fseek(fp, pos, SEEK_SET) != 0) {
        perror("fseek error");
        return -1;
    }

    // Read line from file
    if (fgets(req->data, BUFFERSIZE, fp) == NULL) {
        printf("fgets error\n");
        return -1;
    }

    // Check if end of file is reached
    if (pos + strlen(req->data) >= (fsize = ftell(fp))) {
        req->seqnr = fsize;
        printf("End of File Close sent %ld\n", req->seqnr);
        return -1;
    }

    req->datalen = strlen(req->data);
    req->seqnr = pos;
    req->checksum = checksum((unsigned short*)req, sizeof(struct request));

    pos += strlen(req->data);
    return 0;
}

int main (int argc, char const* argv[]) {
    struct request req;
    struct answer* answ;


    //char Server = argv[1];
   // char File = argv[2];
 
    initclient(NULL);

    FILE* fp = fopen("C:\\Users\\Pandigo\\Desktop\\BelegRNks\\rnks\\rnks\\test.txt", "r");
    if (fp == NULL) {
        perror("fopen");
        fclose(fp);
        return -1;
    }


    while (fillreq(&req, fp) == 0)
    {

        answ = doReq(&req);
        if (answ != 0)
        {
            printf("seqnr: %lu\n", answ->seqnr);
        }
    }


    return 0;

}
