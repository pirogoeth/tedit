#ifndef SIGHANDLER_H
#define SIGHANDLER_H

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

typedef struct SignalCallback {
    int signal;
    void *(callback)(int);
} SignalCallback;

void signal_attach(int signal, void *(callback)(int));
void signal_detach(int signal);
