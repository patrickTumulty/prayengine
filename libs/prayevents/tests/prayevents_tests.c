
#include "prayevents_tests.h"
#include "common_types.h"
#include "prayevents.h"
#include "tmem.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdio.h>

typedef enum : u32
{
    EVENT_A = 0,
    EVENT_B = 1,
    EVENT_C = 2,
} TestEvent;

typedef struct
{
    char word[25];
    char character;
} DataA;


typedef struct
{
    u32 num1;
    u32 num2;
} DataB;

static int eventHandlerACount = 0;
static int eventHandlerBCount = 0;
static int eventHandlerCCount = 0;

static void clearEventCounters()
{
    eventHandlerACount = 0;
    eventHandlerBCount = 0;
    eventHandlerCCount = 0;
}

void handleEventA(Event *event)
{
    CU_ASSERT_PTR_NOT_NULL_FATAL(event);
    CU_ASSERT_PTR_NOT_NULL_FATAL(event->internal);
    CU_ASSERT_PTR_NOT_NULL_FATAL(event->eventData);
    CU_ASSERT_EQUAL_FATAL(event->eventID, EVENT_A);
    CU_ASSERT_EQUAL_FATAL(event->eventDataLen, sizeof(DataA));

    if (event->eventID == EVENT_A)
    {
        eventHandlerACount++;
    }
}

void handleEventB(Event *event)
{
    CU_ASSERT_PTR_NOT_NULL_FATAL(event);
    CU_ASSERT_PTR_NOT_NULL_FATAL(event->internal);
    CU_ASSERT_PTR_NOT_NULL_FATAL(event->eventData);
    CU_ASSERT_EQUAL_FATAL(event->eventID, EVENT_B);
    CU_ASSERT_TRUE_FATAL(event->eventDataLen >= sizeof(DataB));

    if (event->eventID == EVENT_B)
    {
        eventHandlerBCount++;
    }
}

void handleEventC(Event *event)
{
    CU_ASSERT_PTR_NOT_NULL_FATAL(event);
    CU_ASSERT_PTR_NOT_NULL_FATAL(event->internal);
    CU_ASSERT_PTR_NULL_FATAL(event->eventData);
    CU_ASSERT_EQUAL_FATAL(event->eventID, EVENT_C);
    CU_ASSERT_EQUAL_FATAL(event->eventDataLen, 0);

    if (event->eventID == EVENT_C)
    {
        eventHandlerCCount++;
    }
}

static void testEventHandlersCalled(void)
{
    clearEventCounters();

    prayEventsInit();

    Rc rc = prayEventsRegisterHandler(EVENT_A, handleEventA);
    CU_ASSERT_EQUAL(rc, RC_OK);
    rc = prayEventsRegisterHandler(EVENT_B, handleEventB);
    CU_ASSERT_EQUAL(rc, RC_OK);

    Event *eventA = prayEventsGetEvent(EVENT_A, sizeof(DataA));
    CU_ASSERT_PTR_NOT_NULL_FATAL(eventA);
    CU_ASSERT_PTR_NOT_NULL_FATAL(eventA->internal);
    CU_ASSERT_PTR_NOT_NULL_FATAL(eventA->eventData);
    CU_ASSERT_EQUAL_FATAL(eventA->eventID, EVENT_A);
    CU_ASSERT_EQUAL_FATAL(eventA->eventDataLen, sizeof(DataA));

    DataA *dataA = eventA->eventData;
    snprintf(dataA->word, sizeof(dataA->word), "Hello!!!");
    dataA->character = 'a';

    prayEventsPostEvent(eventA);

    Event *eventB = prayEventsGetEvent(EVENT_B, sizeof(DataB));
    CU_ASSERT_PTR_NOT_NULL_FATAL(eventB);
    CU_ASSERT_PTR_NOT_NULL_FATAL(eventB->internal);
    CU_ASSERT_PTR_NOT_NULL_FATAL(eventB->eventData);
    CU_ASSERT_EQUAL_FATAL(eventB->eventID, EVENT_B);
    CU_ASSERT_TRUE_FATAL(eventB->eventDataLen >= sizeof(DataB));

    Event *eventB2 = prayEventsGetEvent(EVENT_B, sizeof(DataB));
    CU_ASSERT_PTR_NOT_NULL_FATAL(eventB2);
    CU_ASSERT_PTR_NOT_NULL_FATAL(eventB2->internal);
    CU_ASSERT_PTR_NOT_NULL_FATAL(eventB2->eventData);
    CU_ASSERT_EQUAL_FATAL(eventB2->eventID, EVENT_B);
    CU_ASSERT_TRUE_FATAL(eventB2->eventDataLen >= sizeof(DataB));

    CU_ASSERT_PTR_NOT_EQUAL_FATAL(eventB, eventB2);

    prayEventsPostEvent(eventB);
    prayEventsPostEvent(eventB2);

    Event *eventC = prayEventsGetEvent(EVENT_C, 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(eventC);
    CU_ASSERT_PTR_NOT_NULL_FATAL(eventC->internal);
    if (eventC->eventData == nullptr)
    {
        CU_ASSERT_TRUE_FATAL(eventC->eventDataLen == 0);
    }
    CU_ASSERT_TRUE_FATAL(eventC->eventDataLen >= 0);
    CU_ASSERT_EQUAL_FATAL(eventC->eventID, EVENT_C);
    CU_ASSERT_TRUE_FATAL(eventC->eventDataLen >= 0);

    prayEventsPostEvent(eventC);

    CU_ASSERT_EQUAL_FATAL(eventHandlerACount, 1);
    CU_ASSERT_EQUAL_FATAL(eventHandlerBCount, 2);
    CU_ASSERT_EQUAL_FATAL(eventHandlerCCount, 0);

    rc = prayEventsUnregisterHandler(EVENT_B, handleEventC);
    CU_ASSERT_EQUAL(rc, RC_NOT_FOUND);

    eventB = prayEventsGetEvent(EVENT_B, sizeof(DataB));
    prayEventsPostEvent(eventB);

    CU_ASSERT_EQUAL_FATAL(eventHandlerBCount, 3);

    rc = prayEventsUnregisterHandler(EVENT_B, handleEventB);
    CU_ASSERT_EQUAL(rc, RC_OK);

    eventB = prayEventsGetEvent(EVENT_B, sizeof(DataB));
    prayEventsPostEvent(eventB);

    CU_ASSERT_EQUAL_FATAL(eventHandlerBCount, 3);

    prayEventsDestroy();

    auto stats = tMemGetStats();
    CU_ASSERT_EQUAL(stats.current, 0);
}

void registerPrayEventsTests()
{
    CU_pSuite suite = CU_add_suite("Pray Events Tests", nullptr, nullptr);

    CU_add_test(suite, "Test Event Handler", testEventHandlersCalled);
}
