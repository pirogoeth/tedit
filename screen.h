#ifndef SCREEN_H
#define SCREEN_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#include "futil.h"

typedef struct Screen {
    WINDOW *window; // ncurses windows

    TFManager *tfm; // file manager

    bool running; // is the screen "active"
    int x, y, width, height; // mutable height/width and starting x/y
} Screen;

void curses_init();

Screen *screen_create(int startcol, int startrow, int width, int height);

void screen_init(Screen *screen);
void screen_nodelay(Screen *screen, bool nodel);
void screen_refresh(Screen *screen);
void screen_deinit(Screen *screen);
void screen_close(Screen *screen);

void screen_draw_frame(Screen *screen, int startrow, int startcol, int width, int height);

void screen_set_active_fmanager(Screen *screen, TFManager *tfm);
TFManager *screen_get_active_fmanager(Screen *screen);
TFile *screen_get_active_file(Screen *screen);
#endif