
#include "pray_system.h"
#include "array_list.h"
#include "tmem.h"
#include <pthread.h>
#include <stdio.h>

void praySystemNoop(void)
{
    // no-op
}

SystemsContext *praySystemsContextNew()
{
    SystemsContext *context = tmemcalloc(1, sizeof(SystemsContext));
    praySystemsContextInit(context);
    return context;
}

void praySystemsContextFree(SystemsContext *context)
{
    praySystemsContextDestroy(context);
    tmemfree(context);
}

void praySystemsContextInit(SystemsContext *context)
{
    alistNew(&context->systems, 0, sizeof(System));
}

void praySystemsContextDestroy(SystemsContext *context)
{
    alistFree(&context->systems);
}


#define SET_NOOP_IF_NULL(A) (A) = (A) != nullptr ? (A) : praySystemNoop

Rc praySystemsRegister(SystemsContext *context, System system)
{
    SET_NOOP_IF_NULL(system.init);
    SET_NOOP_IF_NULL(system.start);
    SET_NOOP_IF_NULL(system.stop);
    SET_NOOP_IF_NULL(system.destroy);
    SET_NOOP_IF_NULL(system.gameUpdate);
    SET_NOOP_IF_NULL(system.renderUpdateScreenSpace);
    SET_NOOP_IF_NULL(system.renderUpdateWorldSpace);
    return alistAppend(&context->systems, &system);
}

void praySystemsRunInit(SystemsContext *context)
{
    for (int i = 0; i < context->systems.length; i++)
    {
        System *system = alistGet(&context->systems, i);
        system->init();
    }
}

void praySystemsRunDestroy(SystemsContext *context)
{
    for (int i = 0; i < context->systems.length; i++)
    {
        System *system = alistGet(&context->systems, i);
        system->destroy();
    }
}

void praySystemsRunStart(SystemsContext *context)
{
    for (int i = 0; i < context->systems.length; i++)
    {
        System *system = alistGet(&context->systems, i);
        system->start();
    }
}

void praySystemsRunStop(SystemsContext *context)
{
    for (int i = 0; i < context->systems.length; i++)
    {
        System *system = alistGet(&context->systems, i);
        system->stop();
    }
}

void praySystemsRunGameUpdate(SystemsContext *context)
{
    for (int i = 0; i < context->systems.length; i++)
    {
        System *system = alistGet(&context->systems, i);
        system->gameUpdate();
    }
}

void praySystemsRunRenderUpdateWorldSpace(SystemsContext *context)
{
    for (int i = 0; i < context->systems.length; i++)
    {
        System *system = alistGet(&context->systems, i);
        system->renderUpdateWorldSpace();
    }
}

void praySystemsRunRenderUpdateScreenSpace(SystemsContext *context)
{
    for (int i = 0; i < context->systems.length; i++)
    {
        System *system = alistGet(&context->systems, i);
        system->renderUpdateScreenSpace();
    }
}
