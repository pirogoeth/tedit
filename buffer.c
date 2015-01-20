#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

#include "buffer.h"
#include "futil.h"

MappedBuffer *buffer_open(TFile *fobj) {
    if (fobj == NULL) {
        return NULL;
    }

    // Allocate memory for the head of the chain.
    MappedBuffer *head = (MappedBuffer *) malloc(sizeof(MappedBuffer));

    // Set link members.
    head->fobj = fobj;
    head->buf = (char *) malloc(BUFFER_BLOCK_LEN);
    // Read data into buffer and set buffer length.
    head->length = read(fobj->fp, head->buf, BUFFER_BLOCK_LEN);

    if (head->length < BUFFER_BLOCK_LEN) { // Did not read full chunk.
        return head;
    }

    // We read a full chunk, we need to keep adding and add to the chain.
    while (1) {
        // Allocate the next link.
        MappedBuffer *link = (MappedBuffer *) malloc(sizeof(MappedBuffer));
        // Set the new link members.
        link->fobj = fobj;
        link->buf = (char *) malloc(BUFFER_BLOCK_LEN);
        link->length = read(fobj->fp, link->buf, BUFFER_BLOCK_LEN);

        // Insert the new link at the end of the chain.
        buffer_insert_tail(head, link);

        // Check the new buffer block length.
        if (link->length < BUFFER_BLOCK_LEN) { // We've read the last chunk, break.
            break;
        }
    }
}

void buffer_close(MappedBuffer *chain) {
    if (chain == NULL) {
        return;
    }

    MappedBuffer *head = buffer_find_head(chain);
    MappedBuffer *nbuf = head;

    while (nbuf != NULL) { // Don't use nbuf->next because we want to clear all links, not stop at the last.
        MappedBuffer *cbuf = nbuf;
        nbuf = nbuf->next;
        free(cbuf);
    }
}

void buffer_write(MappedBuffer *chain) {
}

void buffer_read(MappedBuffer *chain) {
}

MappedBuffer *buffer_find_head(MappedBuffer *buf) {
    if (buf == NULL) {
        return NULL;
    }

    if (buf->prev == NULL) {
        return buf;
    }

    MappedBuffer *pbuf = buf;
    while (pbuf->prev != NULL) {
        pbuf = pbuf->prev;
    }

    return pbuf;
}

MappedBuffer *buffer_find_tail(MappedBuffer *buf) {
    if (buf == NULL) {
        return NULL;
    }

    if (buf->next == NULL) {
        return buf;
    }

    MappedBuffer *nbuf = buf;
    while (nbuf->next != NULL) {
        nbuf = nbuf->next;
    }

    return nbuf;
}

void buffer_insert_head(MappedBuffer *chain, MappedBuffer *buf) {
    if (chain == NULL || buf == NULL) {
        return;
    }

    MappedBuffer *head = buffer_find_head(chain);
    if (head == NULL) {
        return;
    }

    head->prev = buf;
    buf->prev = NULL;
    buf->next = head;
}


void buffer_insert_tail(MappedBuffer *chain, MappedBuffer *buf) {
    if (chain == NULL || buf == NULL) {
        return;
    }

    MappedBuffer *tail = buffer_find_tail(chain);
    if (tail == NULL) {
        return;
    }

    tail->next = buf;
    buf->prev = tail;
    buf->next = NULL;
}
