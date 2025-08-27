
#ifndef PRAYEVENTS_H
#define PRAYEVENTS_H

#include "common_types.h"

typedef struct {
    u32 eventID;
    u32 eventDataLen;
    void *eventData;
} Event;

typedef void(*EventHandlerCallback)(Event event);

Rc prayEventsInit();
Rc prayEventsDestroy();
Rc prayEventsRegisterHandler(u32 eventID, EventHandlerCallback callback);
Rc prayEventsUnregisterHandler(u32 eventID, EventHandlerCallback callback);
Rc prayEventsPostEvent(Event event);


#endif // PRAYEVENTS_H
