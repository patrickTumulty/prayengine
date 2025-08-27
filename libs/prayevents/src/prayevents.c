
#include "prayevents.h"
#include "common_types.h"
#include "pointer_list.h"
#include "pointer_map.h"
#include "tmem.h"

static PMap eventListenersMap;

Rc prayEventsInit()
{
    return pmapNew(&eventListenersMap);
}

Rc prayEventsDestroy()
{
    Rc rc = RC_OK;
    u32 *keys = tmemcalloc(1, sizeof(u32) * eventListenersMap.size);
    if (keys == nullptr)
    {
        return RC_MEM_ALLOC_ERROR;
    }
    rc = pmapGetKeys(&eventListenersMap, keys, eventListenersMap.size);
    if (rc != RC_OK)
    {
        goto EXIT;
    }
    for (int i = 0; i < eventListenersMap.size; i++)
    {
        PList *listenersList = pmapGet(&eventListenersMap, keys[i]);
        plistFree(listenersList);
        tmemfree(listenersList);
    }
    rc = pmapFree(&eventListenersMap);
EXIT:
    tmemfree(keys);
    return rc;
}

Rc prayEventsRegisterHandler(u32 eventID, EventHandlerCallback callback)
{
    // TODO(ptumulty): Ensure duplicate callbacks can't get added
    PList *listenersList = pmapGet(&eventListenersMap, eventID);
    if (listenersList == nullptr)
    {
        listenersList = tmemcalloc(1, sizeof(PList));
        if (listenersList == nullptr)
        {
            return RC_MEM_ALLOC_ERROR;
        }
        Rc rc = plistNew(listenersList, 1);
        if (rc != RC_OK)
        {
            tmemfree(listenersList);
            return rc;
        }
        rc = pmapPut(&eventListenersMap, eventID, listenersList);
        if (rc != RC_OK)
        {
            tmemfree(listenersList);
            return rc;
        }
    }
    return plistAppend(listenersList, callback);
}

Rc prayEventsUnregisterHandler(u32 eventID, EventHandlerCallback callback)
{
    PList *listenersList = pmapGet(&eventListenersMap, eventID);
    if (listenersList == nullptr)
    {
        return RC_NOT_FOUND;
    }
    bool callbackFound = false;
    for (int i = 0; i < listenersList->length; i++)
    {
        EventHandlerCallback cb = plistGet(listenersList, i);
        if (cb == nullptr)
        {
            continue;
        }
        if (cb == callback)
        {
            plistSet(listenersList, i, nullptr);
            callbackFound = true;
            break;
        }
    }
    if (callbackFound)
    {
        plistReduce(listenersList);
        if (listenersList->length == 0)
        {
            plistFree(listenersList);
            tmemfree(listenersList);
            pmapRemove(&eventListenersMap, eventID);
        }
    }
    return callbackFound ? RC_OK : RC_NOT_FOUND;
}

Rc prayEventsPostEvent(Event event)
{
    PList *listenersList = pmapGet(&eventListenersMap, event.eventID);
    if (listenersList == nullptr)
    {
        return RC_NOT_FOUND; // No listeners registered for this event
    }
    for (int i = 0; i < listenersList->length; i++)
    {
        EventHandlerCallback callback = plistGet(listenersList, i);
        if (callback == nullptr)
        {
            continue;
        }
        callback(event);
    }
    return RC_OK;
}
