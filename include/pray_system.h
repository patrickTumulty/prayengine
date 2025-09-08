
#ifndef SYSTEM_H
#define SYSTEM_H

#include "array_list.h"
#include "pray_utils.h"

typedef struct
{
    char name[50];
    Callback init;
    Callback start;
    Callback stop;
    Callback destroy;
    Callback gameUpdate;
    Callback renderUpdateWorldSpace;
    Callback renderUpdateScreenSpace;
} System;

typedef struct {
    AList systems;
} SystemsContext;

void praySystemNoop(void);

SystemsContext *praySystemsContextNew();
void praySystemsContextInit(SystemsContext *context);
void praySystemsContextDestroy(SystemsContext *context);
void praySystemsContextFree(SystemsContext *context);

Rc praySystemsRegister(SystemsContext *context, System system);

void praySystemsRunInit(SystemsContext *context);
void praySystemsRunDestroy(SystemsContext *context);
void praySystemsRunStart(SystemsContext *context);
void praySystemsRunStop(SystemsContext *context);
void praySystemsRunGameUpdate(SystemsContext *context);
void praySystemsRunRenderUpdateWorldSpace(SystemsContext *context);
void praySystemsRunRenderUpdateScreenSpace(SystemsContext *context);

#endif
