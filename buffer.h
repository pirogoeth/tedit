#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "futil.h"

#define BUFFER_BLOCK_LEN 2048
#define BUFFER_READ_BLOCKS 8

typedef struct MappedBuffer MappedBuffer;

typedef struct MappedBuffer {
    char *buf;
    int length;
    TFile *fobj;

    MappedBuffer *prev;
    MappedBuffer *next;
} MappedBuffer;

MappedBuffer *buffer_open(TFile *fobj);
void buffer_close(MappedBuffer *chain);

int buffer_write(MappedBuffer *chain);
void buffer_read(MappedBuffer *chain);

MappedBuffer *buffer_find_head(MappedBuffer *buf);
MappedBuffer *buffer_find_tail(MappedBuffer *buf);

void buffer_insert_head(MappedBuffer *chain, MappedBuffer *buf);
void buffer_insert_tail(MappedBuffer *chain, MappedBuffer *buf);

#endif
