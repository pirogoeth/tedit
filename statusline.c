#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "statusline.h"
#include "screen.h"
#include "futil.h"

static char *replace_string(char *str, char *src, char *dest) {
    char *p;

    if (!(p = strstr(str, src))) {
        return str;
    }

    static char buffer[4096];

    strncpy(buffer, str, p - str);
    buffer[p - str] = '\0';

    sprintf(buffer + (p - str), "%s%s", dest, p + strlen(src));

    return buffer;
}

static char *format_line(Screen *screen, char *str) {
    char *p;
    char tmp[(CHAR_BIT * sizeof(int) - 1) / 3 + 2];

    TFile *fobj = screen_get_active_file(screen);

    p = replace_string(str, "%F", fobj->filepath);

    snprintf(tmp, (CHAR_BIT * sizeof(int) - 1) / 3 + 2, "%ld", fobj->pos);
    p = replace_string(p, "%l", tmp);

    snprintf(tmp, (CHAR_BIT * sizeof(int) - 1) / 3 + 2, "%ld", fobj->len);
    p = replace_string(p, "%L", tmp);

    if ((fobj->pos >= 0) && (fobj->len > 0)) {
        snprintf(tmp, (CHAR_BIT * sizeof(int) - 1) / 3 + 2, "%ld", (fobj->pos / fobj->len));
    } else {
        snprintf(tmp, (CHAR_BIT * sizeof(int) - 1) / 3 + 2, "%d", 0);
    }
    p = replace_string(p, "%PL", tmp);

    return p;
}

Statusline *statusline_create(Screen *screen) {
    Statusline *sl = (Statusline *) malloc(sizeof(Statusline));
    assert(sl != NULL);

    sl->screen = screen;

    sl->format = "[%F] Current: %l/%L  {%PL%}";
    sl->hidden = false;
    sl->row = LINES - 1;

    return sl;
}

void statusline_draw(Statusline *statusline) {
    // perform string replacements.
    char *slstr = format_line(statusline->screen, statusline->format);
    // write statusline to windows
    int saverow, savecol, pos;
    getyx(statusline->screen->window, saverow, savecol);
    wattron(statusline->screen->window, A_REVERSE);
    for (pos = 0; pos < COLS; pos++) {
        if (pos >= strlen(slstr)) {
            mvwaddch(statusline->screen->window, statusline->row, pos, ' ');
        } else if (pos < strlen(slstr)) {
            mvwaddch(statusline->screen->window, statusline->row, pos, slstr[pos]);
        }
    }
    wattroff(statusline->screen->window, A_REVERSE);
    wmove(statusline->screen->window, saverow, savecol);
}
