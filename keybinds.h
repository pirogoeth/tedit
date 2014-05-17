#ifndef KEYBINDS_H
#define KEYBINDS_H

#include <assert.h>
#include <curses.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#define CHARKEY(chr) chr - '0'

typedef struct Keybinder Keybinder;
typedef struct Keybinding Keybinding;

#include "screen.h"

typedef struct Keybinder {
    Keybinding *root;

    Screen *screen;
} Keybinder;

typedef struct Keybinding {
    int key;
    void (*callback)(Screen *, int);

    Keybinding *next;
} Keybinding;

Keybinder *keybinder_create(Screen *screen);
Keybinding *keybinding_create(Keybinder *kb, char key, void (*callback)(Screen *, int));
Keybinding *keybinding_create_raw(Keybinder *kb, int key, void (*callback)(Screen *, int));

void keybinder_unbind(Keybinder *kb, int key);
void keybinder_unbind_all(Keybinder *kb);

void keybinder_process(Keybinder *kb);

#endif