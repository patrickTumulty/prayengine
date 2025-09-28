
#include "praytimer.h"
#include "common_types.h"
#include "common_utils.h"
#include "linked_list.h"
#include "logging_facade.h"
#include "memory_block_manager.h"
#include "tmem.h"

typedef struct
{
    u64 eventTimeCreatedMS;
    u64 delayMS;
    TimerCallback callback;
    LNode node;
} TimerEvent;

typedef struct
{
    LList timerEvents;
    MemoryBlockManager timerEventManager;
} TimerContextInternal;

Rc prayTimerContextInit(TimerContext *context)
{
    TimerContextInternal *internal = tmemcalloc(1, sizeof(TimerContextInternal));
    if (internal == nullptr)
    {
        return RC_MEM_ALLOC_ERROR;
    }

    internal->timerEvents = LListStaticInit();
    Rc rc = memoryBlockManagerInit(&internal->timerEventManager, sizeof(TimerEvent), 3);
    if (rc != RC_OK)
    {
        tmemfree(internal);
        return rc;
    }

    context->internal = internal;

    return RC_OK;
}

Rc prayTimerContextDestroy(TimerContext *context)
{
    TimerContextInternal *internal = context->internal;

    if (internal->timerEvents.size != 0)
    {
        return RC_STILL_IN_USE; // Still have pending timed events
    }

    Rc rc = memoryBlockManagerDestroy(&internal->timerEventManager, true);
    if (rc != RC_OK)
    {
        lfLogError("Error while trying to destroy memory block manager for pray timer context: rc=%s", rc2str(rc));
    }

    tmemfree(internal);
    context->internal = nullptr;

    return RC_OK;
}

Rc prayTimerTick(TimerContext *context)
{


    return RC_OK;
}

Rc prayTimerDelayCallback(TimerContext *context, int delayMS, TimerCallback callback)
{
    return RC_OK;
}
