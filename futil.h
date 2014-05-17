#ifndef FUTIL_H
#define FUTIL_H

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <ncurses.h>

typedef struct TFile {
    FILE *fp; // pointer to file object
    const char *filepath; // name/path of the file
    long len; // length of the file
    long pos; // current position in the file
    bool modified; // if we have locally modified the file
    time_t mtal; // modification time at load

    int lpos; // position in the file array
// hopefully we won't need this.
//    TFile *prev; // previous TFile for maintaining the TF list.
//    TFile *next; // next TFile for maintaining the TF list.
} TFile;

typedef struct TFManager {
    TFile *files; // array of files currently open.
    int open; // number of files currently open.
    int pos; // file that is active
} TFManager;

TFile *file_open(const char *filename);
void file_close(TFile *fobj);

bool file_changed(TFile *fobj);

bool file_save(TFile *fobj);
void file_reload(TFile *fobj);

time_t file_get_mtime(const char *filename);

TFManager *fm_create();
TFile *file_create(TFManager *tfm, const char *filename);
int file_attach(TFManager *tfm, TFile *fobj);
void file_detach(TFManager *tfm, TFile *fobj);
bool file_switch(TFManager *tfm, int pos);
void file_each(TFManager *tfm, void (*callback)(TFManager *, TFile *));
TFile *file_peek(TFManager  *tfm, int pos);

#endif