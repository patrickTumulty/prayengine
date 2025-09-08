
#include "prayevents.h"
#include "common_types.h"
#include "linked_list.h"
#include "pointer_list.h"
#include "pointer_map.h"
#include "tmem.h"
#include <inttypes.h>


typedef struct
{
    LNode lnode;
    bool consumed;
} EventInternal;

#define GET_INTERNAL(EVENT) ((EventInternal *) event->internal)

typedef struct
{
    LList pendingList;
    LList freeList;
    u32 eventID;
    u32 eventDataSize;
} EventQueueContainer;

static PMap eventListenersMap; 
static PMap eventQueueMap; // TODO(ptumulty): make this just a single linked list. No need to make a map for all events

void prayEventConsume(Event *event)
{
    EventInternal *internal = GET_INTERNAL(event);
    internal->consumed = true;
}

static bool prayEventIsConsumed(Event *event)
{
    EventInternal *internal = GET_INTERNAL(event);
    return internal->consumed;
}

Rc prayEventsInit()
{
    Rc rc = pmapNew(&eventListenersMap);
    if (rc != RC_OK)
    {
        return rc;
    }
    rc = pmapNew(&eventQueueMap);
    if (rc != RC_OK)
    {
        pmapFree(&eventListenersMap);
        return rc;
    }
    return RC_OK;
}

Rc cleanupListenersMap()
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

Rc cleanupEventQueues()
{
    Rc rc = RC_OK;
    u32 *keys = tmemcalloc(1, sizeof(u32) * eventQueueMap.size);
    if (keys == nullptr)
    {
        return RC_MEM_ALLOC_ERROR;
    }
    rc = pmapGetKeys(&eventQueueMap, keys, eventQueueMap.size);
    if (rc != RC_OK)
    {
        goto EXIT;
    }
    for (int i = 0; i < eventQueueMap.size; i++)
    {
        EventQueueContainer *eventQueueContainer = pmapGet(&eventQueueMap, keys[i]);
        LNode *lnode = nullptr;
        while ((lnode = llistPopFront(&eventQueueContainer->pendingList)) != nullptr)
        {
            Event *event = lnode->data;
            tmemfree(event);
        }
        while ((lnode = llistPopFront(&eventQueueContainer->freeList)) != nullptr)
        {
            Event *event = lnode->data;
            tmemfree(event);
        }
        tmemfree(eventQueueContainer);
    }
    rc = pmapFree(&eventQueueMap);
EXIT:
    tmemfree(keys);
    return rc;
}

Rc prayEventsDestroy()
{
    cleanupListenersMap();

    cleanupEventQueues();

    return RC_OK;
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

void addEventToFreeList(Event *event)
{
    EventQueueContainer *eventQueueContainer = pmapGet(&eventQueueMap, event->eventID);
    if (eventQueueContainer == nullptr)
    {
        return; // Perhaps this pointer was not managed by the event manager
    }
    EventInternal *internal = GET_INTERNAL(event);
    internal->consumed = false;
    llistRemove(&eventQueueContainer->pendingList, &internal->lnode);
    llistAppend(&eventQueueContainer->freeList, &internal->lnode);
}

Rc prayEventsPostEvent(Event *event)
{
    PList *listenersList = pmapGet(&eventListenersMap, event->eventID);
    if (listenersList == nullptr)
    {
        addEventToFreeList(event);
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
        if (prayEventIsConsumed(event))
        {
            break;
        }
    }
    addEventToFreeList(event);
    return RC_OK;
}

EventQueueContainer *createNewEventQueueContainer(u32 eventID, u32 eventDataSize)
{
    EventQueueContainer *eventQueueContainer = tmemcalloc(1, sizeof(EventQueueContainer));
    if (eventQueueContainer == nullptr)
    {
        return nullptr;
    }
    eventQueueContainer->eventID = eventID;
    eventQueueContainer->eventDataSize = eventDataSize;
    llistInit(&eventQueueContainer->freeList);
    llistInit(&eventQueueContainer->pendingList);
    return eventQueueContainer;
}

Event *createNewEvent(u32 eventID, u32 eventDataSize)
{
    u64 size = sizeof(Event) + sizeof(EventInternal) + eventDataSize;
    void *ptr = tmemcalloc(1, size);
    if (ptr == nullptr)
    {
        return nullptr;
    }
    Event *event = ptr;
    event->eventID = eventID;
    ptr += sizeof(Event);
    event->internal = ptr;
    EventInternal *eventInternal = (EventInternal *) event->internal;
    llistInitNode(&eventInternal->lnode, event);
    if (eventDataSize > 0)
    {
        ptr += sizeof(EventInternal);
        event->eventDataLen = eventDataSize;
        event->eventData = ptr;
    }
    return event;
}

/**
 * @brief Get an event pointer from the pray event manager. This function will
 * allocate new events as needed, but will otherwise hold on to previously used 
 * poitners for reuse. 
 *
 * @param[[in]] eventID event ID 
 * @param[[in]] eventDataSize event attached data size 
 * @return event pointer or nullptr if error 
 */
Event *prayEventsGetEvent(u32 eventID, u32 eventDataSize)
{
    EventQueueContainer *eventQueueContainer = pmapGet(&eventQueueMap, eventID);
    if (eventQueueContainer == nullptr)
    {
        eventQueueContainer = createNewEventQueueContainer(eventID, eventDataSize);
        pmapPut(&eventQueueMap, eventID, eventQueueContainer);
    }
    Event *event = nullptr;
    LNode *lnode = llistPopFront(&eventQueueContainer->freeList);
    if (lnode != nullptr)
    {
        event = (Event *) lnode->data;
    }
    else
    {
        event = createNewEvent(eventID, eventDataSize);
        if (event == nullptr)
        {
            return nullptr;
        }
    }
    EventInternal *internal = GET_INTERNAL(event);
    llistAppend(&eventQueueContainer->pendingList, &internal->lnode);
    return event;
}
