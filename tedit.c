#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <ncurses.h>
#include <unistd.h>

#include "buffer.h"
#include "futil.h"
#include "keybinds.h"
#include "screen.h"
#include "statusline.h"

int main(int argc, char *argv[]) {
    char *filepath;
    filepath = argv[1];
    if (filepath == NULL) {
        printf(" Usage: ./tedit <filename>\n");
        exit(1);
    }

    TFile *infile;
    TFManager *tfm = fm_create();

    infile = file_create(tfm, filepath);

    curses_init();

    Screen *screen = screen_create(0, 0, COLS, LINES);
    screen_set_active_fmanager(screen, tfm);

    Keybinder *kb = keybinder_create(screen);

    Statusline *sl = statusline_create(screen);
    MappedBuffer *bufchain = buffer_open(infile);

    screen_init(screen);

    while (1) {
        // draw screen elements
        statusline_draw(sl);
        // run keybinder and refresh
        keybinder_process(kb);
        screen_refresh(screen);
    }
    return 0;
}
