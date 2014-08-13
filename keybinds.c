#include <assert.h>
#include <curses.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#include "keybinds.h"
#include "screen.h"

Keybinder *keybinder_create(Screen *screen) {
    assert(screen != NULL);
    Keybinder *kb = (Keybinder *) malloc(sizeof(Keybinder));
    assert(kb != NULL);

    kb->root = (Keybinding *) malloc(sizeof(Keybinding));
    kb->screen = screen;

    return kb;
}

Keybinding *keybinding_create(Keybinder *kb, char key, void (*callback)(Screen *, int)) {
    assert(kb != NULL);

//    int keyint = CHARKEY(key);
    int keyint = (int) key;

    return keybinding_create_raw(kb, keyint, callback);
}

Keybinding *keybinding_create_raw(Keybinder *kb, int key, void (*callback)(Screen *, int)) {
    assert(kb != NULL);

    Keybinding *keybind = (Keybinding *) malloc(sizeof(Keybinding));
    assert(keybind != NULL);

    keybind->key = key;
    keybind->callback = callback;

    keybind->next = NULL;

    Keybinding *link = kb->root;
    do {
        if (link->next == NULL) {
            link->next = keybind;
            return keybind;
        }
    } while ((link = link->next) != NULL);

    return keybind;
}

void keybinder_unbind(Keybinder *kb, int key) {
    assert(kb != NULL);

    Keybinding *link = kb->root;
    do {
        if (link->next->key == key) {
            Keybinding *del = link->next;
            Keybinding *next = link->next->next;
            link->next = next;
            del->next = NULL;
            free(del);
            break;
        }
    } while ((link = link->next));
}

void keybinder_unbind_all(Keybinder *kb) {
    assert(kb != NULL);

    Keybinding *link = kb->root;
    while (link != NULL) {
        Keybinding *prev = link;
        link = link->next;
        free(prev);
    }
}

void keybinder_process(Keybinder *kb) {
    assert(kb != NULL);

    int ch = wgetch(kb->screen->window);

    if (ch == ERR) {
        return;
    }

    Keybinding *link = kb->root->next;
    if (link == NULL) {
        return;
    }
    int saverow, savecol;
    do {
        getyx(kb->screen->window, saverow, savecol);
        wmove(kb->screen->window, saverow, savecol);
        if (link->key == ch) {
            link->callback(kb->screen, ch);
        }
        wmove(kb->screen->window, saverow, savecol);
    } while ((link = link->next) != NULL);
}