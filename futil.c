#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <ncurses.h>

#include "futil.h"

TFile *file_open(const char *filename) {
    TFile *fobj = malloc(sizeof(TFile));
    if (!fobj) return NULL;

    fobj->mtal = file_get_mtime(filename); // XXX - probably need to do some checks to verify this worked properly

    if ((fobj->fp = fopen(filename, "r+")) == NULL) {
        if (errno == ENOENT) {
            if ((fobj->fp = fopen(filename, "w+")) == NULL) {
                abort(); // XXX - handle this w/ a switch statement.
            } else {
                fobj->len = 0;
                fobj->pos = 0;
                fobj->modified = TRUE;
            }
        } else {
            abort(); // XXX - handle EACCES, EISDIR, etc.
        }
    } else {
        if (fobj->fp) {
            fobj->pos = 0;

            fseek(fobj->fp, 0L, SEEK_END);
            fobj->len = ftell(fobj->fp);
            rewind(fobj->fp);

            fobj->modified = FALSE;
            fobj->filepath = filename;
        } else {
            abort(); // XXX - deal with whatever condition this is somehow.
        }

        return fobj;
    }

    return NULL;
}

void file_close(TFile *fobj) {
    if (!fobj) {
        return;
    } else {
        if (fobj->modified == TRUE) {
            return; // XXX - figure out how to warn the user and get input on saving.
        } else {
            if (fobj->fp) fclose(fobj->fp);
            free(fobj);
        }
    }
}

bool file_changed(TFile *fobj) {
    if (!fobj) return NULL;

    time_t mtac = file_get_mtime(fobj->filepath);
    if (mtac != fobj->mtal) {
        return TRUE; // file has changed since last read!
    } else {
        return FALSE; // file is the same, clear to save.
    }
}

bool file_save(TFile *fobj) { // XXX - should this be void?
    if (!fobj) return NULL;

    return FALSE; // XXX - finish implementation
}

void file_reload(TFile *fobj) {
    return; // XXX - implement this.
}

time_t file_get_mtime(const char *filename) {
    time_t mtac;

    struct stat statent;
    int statret = stat(filename, &statent); // run stat() so we can grab st_mtim.tv_sec
    if (errno > 0 && statret == -1) {
        // XXX - handle bad stat() calls, maybe use to pre-determine the status of the fopen() call?
        // --- - is this good enough? should we handle more calls?
        switch (errno) {
            case ENOENT:
                return 0;
            default:
                return -1;
        }
    } else {
        mtac = statent.st_mtime; // set our modified time metadata so we can determine later if the file has changed
    }

    return mtac;
}

TFManager *fm_create() {
    TFManager *tfm = (TFManager *) malloc(sizeof(TFManager));
    if (!tfm) return NULL;

    tfm->files = NULL;
    tfm->open = 0;
    tfm->pos = 0;

    return tfm;
}

TFile *file_create(TFManager *tfm, const char *filename) {
    if (!tfm) return NULL;
    TFile *fobj = file_open(filename);
    if (!fobj) return NULL;

    file_attach(tfm, fobj);

    return fobj;
}

int file_attach(TFManager *tfm, TFile *fobj) {
    if (!tfm) return -1;
    if (!fobj) return -1;

    TFile *files = realloc(tfm->files, (tfm->open + 1) * sizeof(TFile)); // reallocate memory for + 1 TFiles
    if (!files) return -1;

    tfm->files = files;

    fobj->lpos = ++tfm->open; // set the file's array position

    free(&tfm->files[tfm->open]); // XXX - should we verify the space is free before allocating it? is this necessary?
    tfm->files[tfm->open] = *fobj;

    tfm->pos = tfm->open; // set the active file index

    return tfm->open;
}

void file_detach(TFManager *tfm, TFile *fobj) { // XXX - should (TFile *fobj) become (int pos) instead?
    return; // XXX - implement this
}

bool file_switch(TFManager *tfm, int pos) { // XXX - should this be void instead?
    return FALSE; // XXX - implement this
}

void file_each(TFManager *tfm, void (*callback)(TFManager *, TFile *)) {
    if (!tfm) return;
    if (!callback) return;

    int i;
    for (i = 0; i < tfm->open; i++) {
        callback(tfm, &tfm->files[i]);
    }
}

TFile *file_peek(TFManager *tfm, int pos) {
    if (!tfm) return NULL;
    if (pos < 0 || pos > tfm->open) return NULL;

    TFile *fobj = &tfm->files[pos];
    if (!fobj) return NULL;

    return fobj;
}
