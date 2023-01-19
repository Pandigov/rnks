#pragma once
#define BUFFERSIZE 100

struct request {
    long datalen;
    unsigned long seqnr;
    char data[BUFFERSIZE];
    unsigned short checksum;
};

struct answer {
    unsigned seqnr;
};