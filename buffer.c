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
    head->length = read(fobj->fp->_file, head->buf, BUFFER_BLOCK_LEN);

    if (head->length < BUFFER_BLOCK_LEN) { // Did not read full chunk.
        return head;
    }

    // We read a full chunk, we need to keep adding and add to the chain.
    for (int i = 1; i < BUFFER_READ_BLOCKS; i++) { // Start at 1 because the head counts as a buffer block.
        // Allocate the next link.
        MappedBuffer *link = (MappedBuffer *) malloc(sizeof(MappedBuffer));
        // Set the new link members.
        link->fobj = fobj;
        link->buf = (char *) malloc(BUFFER_BLOCK_LEN);
        link->length = read(fobj->fp->_file, link->buf, BUFFER_BLOCK_LEN);

        // Insert the new link at the end of the chain.
        buffer_insert_tail(head, link);

        // Check the new buffer block length.
        if (link->length < BUFFER_BLOCK_LEN) { // We've read the last chunk, break.
            break;
        }
    }
    
    return head;
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

int buffer_write(MappedBuffer *chain) {
    if (chain == NULL) {
        return -1;
    }

    MappedBuffer *head = buffer_find_head(chain);

    // Writing the buffer is going to be an interesting process. 
    // First, we should truncate the file to 0, then read back through the buffer starting from head
    // and write it back to file block by block.  Each block is BUFFER_BLOCK_LEN bytes, so each write
    // will be up to that size, with the next block starting at (pos + BUFFER_BLOCK_LEN + 1).  Theoretically,
    // after writing each block, we can fseek(nbuf->fobj->fp->_file, 0L, SEEK_END) and be at the end of the file
    // to start writing the next buffer link.  The only problem is zero termination at the end of the buffer, if
    // applicable.  The buffer may or may not be zero-terminated automatically.  Obviously this is something
    // that has yet to be determined.  Realistically, we should be able to just write(nbuf->fobj->fp->_file, nbuf->buf, nbuf->length)
    // and be done with it.
    
    // The file truncation process may need to be pushed in to platform-dependent code because
    // of the structure of a FILE struct.
    FILE *fobj = head->fobj->fp; // Pull the FILE struct out of the futil/TFile struct.
    rewind(fobj); // Rewind the stream.
    ftruncate(fobj->_file, 0L); // Truncate the file to 0 length.

    MappedBuffer *nbuf = head;

    while (nbuf != NULL) { // Not using nbuf->next since we want to write all links.
        ssize_t wlen = write(fobj->_file, nbuf->buf, nbuf->length); // This may also need to be platformized because of FILE struct.
        if (wlen < nbuf->length) { // What happened? What should we do here? Retry the write?
            return -1;
        }
        nbuf = nbuf->next;
    }

    return 0;
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
