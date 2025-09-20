
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
static int secondHandlerCount = 0;
static int thirdHandlerCount = 0;

static void clearEventCounters()
{
    eventHandlerACount = 0;
    eventHandlerBCount = 0;
    eventHandlerCCount = 0;
    secondHandlerCount = 0;
    thirdHandlerCount = 0;
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

void handleEventASecond(Event *event)
{
    CU_ASSERT_PTR_NOT_NULL_FATAL(event);
    CU_ASSERT_EQUAL_FATAL(event->eventID, EVENT_A);
    secondHandlerCount++;
}

void handleEventAThird(Event *event)
{
    CU_ASSERT_PTR_NOT_NULL_FATAL(event);
    CU_ASSERT_EQUAL_FATAL(event->eventID, EVENT_A);
    thirdHandlerCount++;
    prayEventConsume(event);
}

static void testMultipleHandlersOnSameEvent(void)
{
    clearEventCounters();

    prayEventsInit();

    Rc rc = prayEventsRegisterHandler(EVENT_A, handleEventA);
    CU_ASSERT_EQUAL(rc, RC_OK);
    rc = prayEventsRegisterHandler(EVENT_A, handleEventASecond);
    CU_ASSERT_EQUAL(rc, RC_OK);
    rc = prayEventsRegisterHandler(EVENT_A, handleEventAThird);
    CU_ASSERT_EQUAL(rc, RC_OK);

    Event *eventA = prayEventsGetEvent(EVENT_A, sizeof(DataA));
    CU_ASSERT_PTR_NOT_NULL_FATAL(eventA);
    
    prayEventsPostEvent(eventA);

    CU_ASSERT_EQUAL_FATAL(eventHandlerACount, 1);
    CU_ASSERT_EQUAL_FATAL(secondHandlerCount, 1);
    CU_ASSERT_EQUAL_FATAL(thirdHandlerCount, 1);

    Event *eventA2 = prayEventsGetEvent(EVENT_A, sizeof(DataA));
    CU_ASSERT_PTR_NOT_NULL_FATAL(eventA2);
    
    prayEventsPostEvent(eventA2);

    CU_ASSERT_EQUAL_FATAL(eventHandlerACount, 2);
    CU_ASSERT_EQUAL_FATAL(secondHandlerCount, 2);
    CU_ASSERT_EQUAL_FATAL(thirdHandlerCount, 2);

    prayEventsDestroy();

    auto stats = tMemGetStats();
    CU_ASSERT_EQUAL(stats.current, 0);
}

static void testEventConsumeStopsHandlerChain(void)
{
    clearEventCounters();

    prayEventsInit();

    Rc rc = prayEventsRegisterHandler(EVENT_A, handleEventA);
    CU_ASSERT_EQUAL(rc, RC_OK);
    
    Event *eventA = prayEventsGetEvent(EVENT_A, sizeof(DataA));
    CU_ASSERT_PTR_NOT_NULL_FATAL(eventA);
    
    DataA *dataA = eventA->eventData;
    snprintf(dataA->word, sizeof(dataA->word), "Test");
    dataA->character = 't';

    prayEventConsume(eventA);
    prayEventsPostEvent(eventA);

    CU_ASSERT_EQUAL_FATAL(eventHandlerACount, 1);

    prayEventsDestroy();
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

static void testEventReuseAndMemoryManagement(void)
{
    prayEventsInit();

    Event *eventA1 = prayEventsGetEvent(EVENT_A, sizeof(DataA));
    CU_ASSERT_PTR_NOT_NULL_FATAL(eventA1);
    void *firstEventPtr = eventA1;
    
    prayEventsPostEvent(eventA1);

    Event *eventA2 = prayEventsGetEvent(EVENT_A, sizeof(DataA));
    CU_ASSERT_PTR_NOT_NULL_FATAL(eventA2);
    CU_ASSERT_PTR_EQUAL(firstEventPtr, eventA2);

    Event *eventLarger = prayEventsGetEvent(EVENT_A, sizeof(DataA) * 2);
    CU_ASSERT_PTR_NOT_NULL_FATAL(eventLarger);
    CU_ASSERT_TRUE(eventLarger->eventDataLen >= sizeof(DataA) * 2);
    
    prayEventsPostEvent(eventA2);
    prayEventsPostEvent(eventLarger);

    Event *eventSmaller = prayEventsGetEvent(EVENT_A, sizeof(DataA));
    CU_ASSERT_PTR_NOT_NULL_FATAL(eventSmaller);
    CU_ASSERT_TRUE(eventSmaller->eventDataLen >= sizeof(DataA));

    prayEventsPostEvent(eventSmaller);

    prayEventsDestroy();

    auto stats = tMemGetStats();
    CU_ASSERT_EQUAL(stats.current, 0);
}

static void testPostEventWithNoListeners(void)
{
    prayEventsInit();

    Event *eventA = prayEventsGetEvent(EVENT_A, sizeof(DataA));
    CU_ASSERT_PTR_NOT_NULL_FATAL(eventA);
    
    Rc rc = prayEventsPostEvent(eventA);
    CU_ASSERT_EQUAL(rc, RC_NOT_FOUND);

    prayEventsDestroy();

    auto stats = tMemGetStats();
    CU_ASSERT_EQUAL(stats.current, 0);
}

static void testDuplicateHandlerRegistration(void)
{
    clearEventCounters();

    prayEventsInit();

    Rc rc = prayEventsRegisterHandler(EVENT_A, handleEventA);
    CU_ASSERT_EQUAL(rc, RC_OK);
    
    rc = prayEventsRegisterHandler(EVENT_A, handleEventA);
    CU_ASSERT_EQUAL(rc, RC_OK);

    Event *eventA = prayEventsGetEvent(EVENT_A, sizeof(DataA));
    CU_ASSERT_PTR_NOT_NULL_FATAL(eventA);
    
    prayEventsPostEvent(eventA);

    CU_ASSERT_EQUAL_FATAL(eventHandlerACount, 2);

    prayEventsDestroy();

    auto stats = tMemGetStats();
    CU_ASSERT_EQUAL(stats.current, 0);
}

static void testInitDestroyMultipleTimes(void)
{
    Rc rc = prayEventsInit();
    CU_ASSERT_EQUAL(rc, RC_OK);

    rc = prayEventsInit();
    CU_ASSERT_EQUAL(rc, RC_ALREADY_DONE);
    
    rc = prayEventsDestroy();
    CU_ASSERT_EQUAL(rc, RC_OK);

    rc = prayEventsInit();
    CU_ASSERT_EQUAL(rc, RC_OK);
    
    rc = prayEventsDestroy();
    CU_ASSERT_EQUAL(rc, RC_OK);

    auto stats = tMemGetStats();
    CU_ASSERT_EQUAL(stats.current, 0);
}

void registerPrayEventsTests()
{
    CU_pSuite suite = CU_add_suite("Pray Events Tests", nullptr, nullptr);

    CU_add_test(suite, "Test Event Handler", testEventHandlersCalled);
    CU_add_test(suite, "Test Event Consume Stops Handler Chain", testEventConsumeStopsHandlerChain);
    CU_add_test(suite, "Test Event Reuse And Memory Management", testEventReuseAndMemoryManagement);
    CU_add_test(suite, "Test Multiple Handlers On Same Event", testMultipleHandlersOnSameEvent);
    CU_add_test(suite, "Test Post Event With No Listeners", testPostEventWithNoListeners);
    CU_add_test(suite, "Test Duplicate Handler Registration", testDuplicateHandlerRegistration);
    CU_add_test(suite, "Test Init Destroy Multiple Times", testInitDestroyMultipleTimes);
}
