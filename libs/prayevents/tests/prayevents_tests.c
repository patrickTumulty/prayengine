
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

static int eventHandlerACount = 0;
static int eventHandlerBCount = 0;
static int eventHandlerCCount = 0;

static void clearEventCounters()
{
    eventHandlerACount = 0;
    eventHandlerBCount = 0;
    eventHandlerCCount = 0;
}

void handleEventA(Event event)
{
    if (event.eventID == EVENT_A)
    {
        eventHandlerACount++;
    }
}

void handleEventB(Event event)
{
    if (event.eventID == EVENT_B)
    {
        eventHandlerBCount++;
    }
}

void handleEventC(Event event)
{
    if (event.eventID == EVENT_C)
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

    prayEventsPostEvent((Event) {
        .eventID = EVENT_A,
        .eventData = nullptr,
        .eventDataLen = 0,
    });

    prayEventsPostEvent((Event) {
        .eventID = EVENT_B,
        .eventData = nullptr,
        .eventDataLen = 0,
    });

    prayEventsPostEvent((Event) {
        .eventID = EVENT_B,
        .eventData = nullptr,
        .eventDataLen = 0,
    });

    prayEventsPostEvent((Event) {
        .eventID = EVENT_C,
        .eventData = nullptr,
        .eventDataLen = 0,
    });

    CU_ASSERT_EQUAL_FATAL(eventHandlerACount, 1);
    CU_ASSERT_EQUAL_FATAL(eventHandlerBCount, 2);
    CU_ASSERT_EQUAL_FATAL(eventHandlerCCount, 0);

    rc = prayEventsUnregisterHandler(EVENT_B, handleEventC);
    CU_ASSERT_EQUAL(rc, RC_NOT_FOUND);

    prayEventsPostEvent((Event) {
        .eventID = EVENT_B,
        .eventData = nullptr,
        .eventDataLen = 0,
    });

    CU_ASSERT_EQUAL_FATAL(eventHandlerBCount, 3);

    rc = prayEventsUnregisterHandler(EVENT_B, handleEventB);
    CU_ASSERT_EQUAL(rc, RC_OK);

    prayEventsPostEvent((Event) {
        .eventID = EVENT_B,
        .eventData = nullptr,
        .eventDataLen = 0,
    });

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

