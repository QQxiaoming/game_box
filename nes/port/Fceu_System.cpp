#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fceu.h"
#include "mainwindow.h"
#include "game_box_misc.h"

static NESThread *g_nesThread;

void Fceu_start(NESThread *nesThread, const char *pszFileName) {
    g_nesThread = nesThread;
    g_nesThread->libVersion = "V0.98.12";

    FCEUI_Initialize();
}
