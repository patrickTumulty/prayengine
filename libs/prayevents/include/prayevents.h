
#ifndef PRAYEVENTS_H
#define PRAYEVENTS_H

#include "common_types.h"

typedef struct {
    u32 eventID;
    void *internal;
    u32 eventDataLen;
    void *eventData;
} Event;

typedef void(*EventHandlerCallback)(Event *event);

void prayEventConsume(Event *event);
Rc prayEventsInit();
Rc prayEventsDestroy();
Rc prayEventsRegisterHandler(u32 eventID, EventHandlerCallback callback);
Rc prayEventsUnregisterHandler(u32 eventID, EventHandlerCallback callback);
Rc prayEventsPostEvent(Event *event);
Event *prayEventsGetEvent(u32 eventID, u32 eventDataSize);


#endif // PRAYEVENTS_H
