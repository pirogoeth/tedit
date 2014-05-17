#ifndef STATUSLINE_H
#define STATUSLINE_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "screen.h"

typedef struct Statusline {
    Screen *screen;

    char *format;

    bool hidden;
    int row;
} Statusline;

static char *replace_string(char *orig, char *from, char *to);
static char *format_line(Screen *screen, char *str);

Statusline *statusline_create(Screen *screen);
void statusline_draw(Statusline *statusline);
#endif