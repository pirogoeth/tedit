#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#include "screen.h"
#include "futil.h"

void curses_init() {
    initscr();
    cbreak();
    noecho();
    qiflush();
    nonl();
}

Screen *screen_create(int x, int y, int width, int height) {
    Screen *screen = (Screen *) malloc(sizeof(Screen));
    assert(screen != NULL);

    screen->x = x;
    screen->y = y;
    screen->width = width;
    screen->height = height;

    WINDOW *window = newwin(height, width, y, x);

    screen->window = window;
    screen->running = false;

    return screen;
}

void screen_init(Screen *screen) {
    if (screen->running == false) {
        screen_nodelay(screen, true);
        intrflush(screen->window, false);
        keypad(screen->window, true);

        screen->running = true;
    }
}

void screen_refresh(Screen *screen) {
    if (screen->running) {
        wrefresh(screen->window);
    }
}

void screen_nodelay(Screen *screen, bool nodel) {
    if (screen->running) {
        nodelay(screen->window, nodel);
    }
}

void screen_deinit(Screen *screen) {
    if (screen->running) {
        endwin();
        werase(screen->window);
    }
}

void screen_close(Screen *screen) {
    if (screen->running) {
        screen_deinit(screen);
        exit(0);
    }
}

void screen_draw_frame(Screen *screen, int startrow, int startcol, int width, int height) {
    int saverow, savecol, pos;
    getyx(screen->window, saverow, savecol);
    wattron(screen->window, A_REVERSE);
    mvwaddch(screen->window, startrow, startcol, ACS_ULCORNER);
    for (pos = startcol + 1; pos < height; pos++) {
        waddch(screen->window, ACS_HLINE);
    }
    waddch(screen->window, ACS_URCORNER);
    for (pos = startrow + 1; pos < width; pos++) {
        mvwaddch(screen->window, pos, startcol, ACS_VLINE);
        mvwaddch(screen->window, pos, height, ACS_VLINE);
    }
    mvwaddch(screen->window, width, startcol, ACS_LLCORNER);
    for (pos = startcol + 1; pos < height; pos++) {
        waddch(screen->window, ACS_HLINE);
    }
    waddch(screen->window, ACS_LRCORNER);
    wattroff(screen->window, A_REVERSE);
    wmove(screen->window, saverow, savecol);
}

void screen_set_active_fmanager(Screen *screen, TFManager *tfm) {
    if (!screen) return;
    if (!tfm) return;

    screen->tfm = tfm;
}

TFManager *screen_get_active_fmanager(Screen *screen) {
    if (!screen) return NULL;

    return screen->tfm;
}

TFile *screen_get_active_file(Screen *screen) {
    if (!screen) return NULL;

    return file_peek(screen->tfm, screen->tfm->pos);
}