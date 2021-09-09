#ifndef GAME_BOX_MISC
#define GAME_BOX_MISC

#include <QCoreApplication>

#if defined(Q_OS_WIN)
#include <windows.h>
int gettimeofday(struct timeval *tv, struct timezone *tz);
#endif

#if defined(Q_OS_LINUX)
#include <sys/time.h>
#endif

#if defined(Q_OS_MACOS)
#include <sys/time.h>
#endif

#endif /* GAME_BOX_MISC */
