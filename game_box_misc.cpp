#include "game_box_misc.h"

#if defined(Q_OS_WIN)

int gettimeofday(struct timeval *tv, struct timezone *tz) {
    time_t rawtime;

    time(&rawtime);
    tv->tv_sec = (long)rawtime;

    // here starts the microsecond resolution:

    LARGE_INTEGER tickPerSecond;
    LARGE_INTEGER tick; // a point in time

    // get the high resolution counter's accuracy
    QueryPerformanceFrequency(&tickPerSecond);

    // what time is it ?
    QueryPerformanceCounter(&tick);

    // and here we get the current microsecond! \o/
    tv->tv_usec = (tick.QuadPart % tickPerSecond.QuadPart);

    Q_UNUSED(tz);
    return 0;
}
#endif // _WIN32_
