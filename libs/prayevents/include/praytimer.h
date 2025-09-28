
#ifndef PRAYTIMER_H
#define PRAYTIMER_H

#include "common_types.h"

typedef struct
{
    void *internal;
} TimerContext;

typedef void(*TimerCallback)(void *data);

Rc prayTimerContextInit(TimerContext *context);
Rc prayTimerContextDestroy(TimerContext *context);
Rc prayTimerTick(TimerContext *context);
Rc prayTimerDelayCallback(TimerContext *context, int delayMS, TimerCallback callback);

#endif // PRAYTIMER_H
