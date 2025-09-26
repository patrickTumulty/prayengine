
#include "common_hash.h"
#include "common_tests.h"
#include "common_types.h"
#include "pointer_list.h"
#include "pointer_map.h"
#include "tmem.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <pthread.h>
#include <stdio.h>


typedef struct
{
    char str[15];
    int id;
} TestStruct;

static TestStruct testStructs[100] = {};
static u32 testStructsHashes[100] = {};


static int setup(void)
{
    tMemInit();

    for (int i = 0; i < 100; i++)
    {
        TestStruct *ts = &testStructs[i];
        snprintf(ts->str, sizeof(ts->str), "Struct%d", i);
        ts->id = i;
        testStructsHashes[i] = hashFNV_1A_32((u8 *) ts->str, strlen(ts->str));
    }


    return 0;
}

static int teardown(void)
{
    tMemDestroy();
    return 0;
}

void pmapAddingRemoveingElement(void)
{
    TestStruct *ts0 = &testStructs[0];
    TestStruct *ts1 = &testStructs[1];
    TestStruct *ts2 = &testStructs[2];
    u32 ts0_hash = testStructsHashes[0];
    u32 ts1_hash = testStructsHashes[1];
    u32 ts2_hash = testStructsHashes[2];

    PMap pmap = {0};
    Rc rc = pmapNew(&pmap);
    CU_ASSERT_EQUAL(rc, RC_OK);
    CU_ASSERT_EQUAL(pmap.size, 0);

    rc = pmapPut(&pmap, ts0_hash, ts0);
    CU_ASSERT_EQUAL(rc, RC_OK);
    CU_ASSERT_EQUAL(pmap.size, 1);

    rc = pmapPut(&pmap, ts1_hash, ts1);
    CU_ASSERT_EQUAL(rc, RC_OK);
    CU_ASSERT_EQUAL(pmap.size, 2);

    rc = pmapPut(&pmap, ts2_hash, ts2);
    CU_ASSERT_EQUAL(rc, RC_OK);
    CU_ASSERT_EQUAL(pmap.size, 3);

    TestStruct *ts2_alt = pmapRemove(&pmap, ts2_hash);
    CU_ASSERT_PTR_NOT_NULL(ts2_alt);
    CU_ASSERT_PTR_EQUAL(ts2_alt, ts2);
    CU_ASSERT_EQUAL(pmap.size, 2);

    TestStruct *ts1_alt = pmapRemove(&pmap, ts1_hash);
    CU_ASSERT_PTR_NOT_NULL(ts1_alt);
    CU_ASSERT_PTR_EQUAL(ts1_alt, ts1);
    CU_ASSERT_EQUAL(pmap.size, 1);

    ts1_alt = pmapGet(&pmap, ts1_hash);
    CU_ASSERT_PTR_NULL(ts1_alt);
    CU_ASSERT_EQUAL(pmap.size, 1);

    TestStruct *ts0_alt = pmapRemove(&pmap, ts0_hash);
    CU_ASSERT_PTR_NOT_NULL(ts0_alt);
    CU_ASSERT_PTR_EQUAL(ts0_alt, ts0);
    CU_ASSERT_EQUAL(pmap.size, 0);

    rc = pmapFree(&pmap);
    CU_ASSERT_EQUAL(rc, RC_OK);

    auto stats = tMemGetStats();
    CU_ASSERT_EQUAL(stats.current, 0);
}

void pmapAddingRemoveingDuplicateElement(void)
{
    TestStruct *ts0 = &testStructs[0];
    TestStruct *ts1 = &testStructs[1];
    u32 ts0_hash = testStructsHashes[0];
    u32 ts1_hash = testStructsHashes[1];

    PMap pmap = {0};
    Rc rc = pmapNew(&pmap);
    CU_ASSERT_EQUAL(rc, RC_OK);
    CU_ASSERT_EQUAL(pmap.size, 0);

    rc = pmapPut(&pmap, ts0_hash, ts0);
    CU_ASSERT_EQUAL(rc, RC_OK);
    CU_ASSERT_EQUAL(pmap.size, 1);

    rc = pmapPut(&pmap, ts0_hash, ts0);
    CU_ASSERT_NOT_EQUAL(rc, RC_OK);
    CU_ASSERT_EQUAL(pmap.size, 1);

    rc = pmapPut(&pmap, ts1_hash, ts1);
    CU_ASSERT_EQUAL(rc, RC_OK);
    CU_ASSERT_EQUAL(pmap.size, 2);

    TestStruct *ts0_alt = pmapRemove(&pmap, ts0_hash);
    CU_ASSERT_PTR_NOT_NULL(ts0_alt);
    CU_ASSERT_PTR_EQUAL(ts0_alt, ts0);
    CU_ASSERT_EQUAL(pmap.size, 1);

    ts0_alt = pmapRemove(&pmap, ts0_hash);
    CU_ASSERT_PTR_NULL(ts0_alt);
    CU_ASSERT_EQUAL(pmap.size, 1);

    rc = pmapFree(&pmap);
    CU_ASSERT_EQUAL(rc, RC_OK);

    auto stats = tMemGetStats();
    CU_ASSERT_EQUAL(stats.current, 0);
}

void pmapResizeTest(void)
{
    PMap pmap = {0};
    Rc rc = pmapNew(&pmap);
    CU_ASSERT_EQUAL(rc, RC_OK);
    CU_ASSERT_EQUAL(pmap.size, 0);

    for (int i = 0; i < 50; i++)
    {
        TestStruct *ts = &testStructs[i];
        u32 tsHash = testStructsHashes[i];
        rc = pmapPut(&pmap, tsHash, ts);
        CU_ASSERT_EQUAL_FATAL(rc, RC_OK);
        CU_ASSERT_EQUAL_FATAL(pmap.size, i + 1);
    }

    TestStruct *ts17 = &testStructs[17];
    u32 ts17Hash = testStructsHashes[17];

    TestStruct *ts17_alt = pmapGet(&pmap, ts17Hash);
    CU_ASSERT_PTR_NOT_NULL(ts17_alt);
    CU_ASSERT_PTR_EQUAL(ts17_alt, ts17);

    TestStruct *ts84_alt = pmapGet(&pmap, testStructsHashes[84]);
    CU_ASSERT_PTR_NULL(ts84_alt);

    rc = pmapFree(&pmap);
    CU_ASSERT_EQUAL(rc, RC_OK);

    auto stats = tMemGetStats();
    CU_ASSERT_EQUAL(stats.current, 0);
}

u64 getTimestampNanos()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (u64) (ts.tv_sec * 1000000000UL) + ts.tv_nsec;
}

void pmapPerformanceTest(void)
{
    PMap pmap = {};
    Rc rc = pmapNew(&pmap);
    CU_ASSERT_EQUAL(rc, RC_OK);
    CU_ASSERT_EQUAL(pmap.size, 0);

    PList plist = {};
    rc = plistNew(&plist, 100);

    srand(time(nullptr));

    u32 queryList[100] = {0};
    for (int i = 0; i < 100; i++)
    {
        pmapPut(&pmap, testStructsHashes[i], &testStructs[i]);
        plistSet(&plist, i, &testStructs[i]);
        queryList[i] = rand() % 100;
    }

    printf("\nStarting Performance Test:\n");

    u64 t1 = getTimestampNanos();

    for (int i = 0; i < 100; i++)
    {
        TestStruct *target = &testStructs[queryList[i]];
        u32 targetHash = testStructsHashes[queryList[i]];
        TestStruct *s = pmapGet(&pmap, targetHash);
        CU_ASSERT_PTR_NOT_NULL_FATAL(s);
        CU_ASSERT_PTR_EQUAL_FATAL(s, target)
    }

    u64 t2 = getTimestampNanos();
    u64 mapTime = t2 - t1;

    t1 = getTimestampNanos();

    for (int i = 0; i < 100; i++)
    {
        TestStruct *target = &testStructs[queryList[i]];
        TestStruct *s = nullptr;
        for (int j = 0; j < plist.length; j++)
        {
            s = plistGet(&plist, j);
            if (s->id == target->id)
            {
                break;
            }
        }
        CU_ASSERT_PTR_NOT_NULL_FATAL(s);
        CU_ASSERT_PTR_EQUAL_FATAL(s, target)
    }

    t2 = getTimestampNanos();
    printf("Map Time   : %lu ns\n", mapTime);
    printf("Array Time : %lu ns\n", t2 - t1);
}


void pmapGetKeysTest(void)
{
    PMap pmap = {0};
    Rc rc = pmapNew(&pmap);
    CU_ASSERT_EQUAL(rc, RC_OK);
    CU_ASSERT_EQUAL(pmap.size, 0);

    u32 hashes[30] = { 0 };

    for (int i = 0; i < 30; i++)
    {
        TestStruct *ts = &testStructs[i];
        u32 tsHash = testStructsHashes[i];
        hashes[i] = tsHash;
        rc = pmapPut(&pmap, tsHash, ts);
        CU_ASSERT_EQUAL_FATAL(rc, RC_OK);
        CU_ASSERT_EQUAL_FATAL(pmap.size, i + 1);
    }

    u32 keys[30] = { 0 };
    u32 keysCount = 0;
    rc = pmapGetKeys(&pmap, keys, 30, &keysCount);
    CU_ASSERT_EQUAL(rc, RC_OK);
    CU_ASSERT_EQUAL(keysCount, 30);

    for (int i = 0; i < 30; i++)
    {
        for (int j = 0; j < 30; j++)
        {
            if (keys[i] == hashes[j])
            {
                hashes[j] = 0;
            }
        }
    }

    for (int i = 0; i < 30; i++)
    {
        CU_ASSERT_EQUAL_FATAL(hashes[i], 0);
    }

    rc = pmapFree(&pmap);
    CU_ASSERT_EQUAL(rc, RC_OK);

    auto stats = tMemGetStats();
    CU_ASSERT_EQUAL(stats.current, 0);
}

void registerPMapTests(void)
{
    CU_pSuite suite = CU_add_suite("Pointer Map Tests", setup, teardown);
    CU_add_test(suite, "Adding/Removing Elements", pmapAddingRemoveingElement);
    CU_add_test(suite, "Adding/Removing Duplicate Elements", pmapAddingRemoveingDuplicateElement);
    CU_add_test(suite, "Resize Test", pmapResizeTest);
    CU_add_test(suite, "Get Keys Test", pmapGetKeysTest);
    CU_add_test(suite, "Performance Test", pmapPerformanceTest);
}
