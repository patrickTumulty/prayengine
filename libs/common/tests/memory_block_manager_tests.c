#include "common_tests.h"
#include "common_types.h"
#include "memory_block_manager.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdint.h>

void memoryBlockManagerInitTest(void)
{
    MemoryBlockManager manager;
    Rc result = memoryBlockManagerInit(&manager, 256, 5);

    CU_ASSERT_EQUAL(result, RC_OK);
    CU_ASSERT_PTR_NOT_NULL(manager.internal);

    result = memoryBlockManagerDestroy(&manager, false);
    CU_ASSERT_EQUAL(result, RC_OK);
    CU_ASSERT_PTR_NULL(manager.internal);
}

void memoryBlockManagerInitZeroBlocksTest(void)
{
    MemoryBlockManager manager;
    Rc result = memoryBlockManagerInit(&manager, 128, 0);

    CU_ASSERT_EQUAL(result, RC_OK);
    CU_ASSERT_PTR_NOT_NULL(manager.internal);

    result = memoryBlockManagerDestroy(&manager, false);
    CU_ASSERT_EQUAL(result, RC_OK);
}

void memoryBlockManagerGetPtrTest(void)
{
    MemoryBlockManager manager;
    Rc result = memoryBlockManagerInit(&manager, 64, 2);
    CU_ASSERT_EQUAL(result, RC_OK);

    void *ptr1 = memoryBlockManagerGetPtr(&manager);
    CU_ASSERT_PTR_NOT_NULL(ptr1);

    void *ptr2 = memoryBlockManagerGetPtr(&manager);
    CU_ASSERT_PTR_NOT_NULL(ptr2);
    CU_ASSERT_PTR_NOT_EQUAL(ptr1, ptr2);

    void *ptr3 = memoryBlockManagerGetPtr(&manager);
    CU_ASSERT_PTR_NOT_NULL(ptr3);

    result = memoryBlockManagerReleasePtr(&manager, ptr1);
    CU_ASSERT_EQUAL(result, RC_OK);

    result = memoryBlockManagerReleasePtr(&manager, ptr2);
    CU_ASSERT_EQUAL(result, RC_OK);

    result = memoryBlockManagerReleasePtr(&manager, ptr3);
    CU_ASSERT_EQUAL(result, RC_OK);

    result = memoryBlockManagerDestroy(&manager, false);
    CU_ASSERT_EQUAL(result, RC_OK);
}

void memoryBlockManagerGetPtrNullManagerTest(void)
{
    void *ptr = memoryBlockManagerGetPtr(nullptr);
    CU_ASSERT_PTR_NULL(ptr);
}

void memoryBlockManagerReleasePtrTest(void)
{
    MemoryBlockManager manager;
    Rc result = memoryBlockManagerInit(&manager, 128, 1);
    CU_ASSERT_EQUAL(result, RC_OK);

    void *ptr = memoryBlockManagerGetPtr(&manager);
    CU_ASSERT_PTR_NOT_NULL(ptr);

    result = memoryBlockManagerReleasePtr(&manager, ptr);
    CU_ASSERT_EQUAL(result, RC_OK);

    void *ptr2 = memoryBlockManagerGetPtr(&manager);
    CU_ASSERT_PTR_NOT_NULL(ptr2);
    CU_ASSERT_PTR_EQUAL(ptr, ptr2);

    result = memoryBlockManagerReleasePtr(&manager, ptr2);
    CU_ASSERT_EQUAL(result, RC_OK);

    result = memoryBlockManagerDestroy(&manager, false);
    CU_ASSERT_EQUAL(result, RC_OK);
}

void memoryBlockManagerReleasePtrBadParamsTest(void)
{
    MemoryBlockManager manager;
    Rc result = memoryBlockManagerInit(&manager, 64, 1);
    CU_ASSERT_EQUAL(result, RC_OK);

    result = memoryBlockManagerReleasePtr(nullptr, nullptr);
    CU_ASSERT_EQUAL(result, RC_BAD_PARAM);

    result = memoryBlockManagerReleasePtr(&manager, nullptr);
    CU_ASSERT_EQUAL(result, RC_BAD_PARAM);

    result = memoryBlockManagerDestroy(&manager, false);
    CU_ASSERT_EQUAL(result, RC_OK);
}

void memoryBlockManagerDestroyTest(void)
{
    MemoryBlockManager manager;
    Rc result = memoryBlockManagerInit(&manager, 256, 3);
    CU_ASSERT_EQUAL(result, RC_OK);

    result = memoryBlockManagerDestroy(&manager, false);
    CU_ASSERT_EQUAL(result, RC_OK);
    CU_ASSERT_PTR_NULL(manager.internal);
}

void memoryBlockManagerDestroyNullTest(void)
{
    Rc result = memoryBlockManagerDestroy(nullptr, false);
    CU_ASSERT_EQUAL(result, RC_BAD_PARAM);
}

void memoryBlockManagerDestroyWithActiveBlocksTest(void)
{
    MemoryBlockManager manager;
    Rc result = memoryBlockManagerInit(&manager, 128, 2);
    CU_ASSERT_EQUAL(result, RC_OK);

    void *ptr = memoryBlockManagerGetPtr(&manager);
    CU_ASSERT_PTR_NOT_NULL(ptr);

    result = memoryBlockManagerDestroy(&manager, false);
    CU_ASSERT_EQUAL(result, RC_NOT_ALLOWED);

    result = memoryBlockManagerDestroy(&manager, true);
    CU_ASSERT_EQUAL(result, RC_OK);
    CU_ASSERT_PTR_NULL(manager.internal);
}

void memoryBlockManagerMultipleAllocReleaseTest(void)
{
    MemoryBlockManager manager;
    Rc result = memoryBlockManagerInit(&manager, 512, 1);
    CU_ASSERT_EQUAL(result, RC_OK);

    void *ptrs[10];

    for (int i = 0; i < 10; i++)
    {
        ptrs[i] = memoryBlockManagerGetPtr(&manager);
        CU_ASSERT_PTR_NOT_NULL(ptrs[i]);
    }

    for (int i = 0; i < 10; i++)
    {
        result = memoryBlockManagerReleasePtr(&manager, ptrs[i]);
        CU_ASSERT_EQUAL(result, RC_OK);
    }

    result = memoryBlockManagerDestroy(&manager, false);
    CU_ASSERT_EQUAL(result, RC_OK);
}

void registerMemoryBlockManagerTests(void)
{
    CU_pSuite pSuite = nullptr;

    pSuite = CU_add_suite("Memory Block Manager Tests", nullptr, nullptr);
    if (nullptr == pSuite)
    {
        CU_cleanup_registry();
        return;
    }

    if (nullptr == CU_add_test(pSuite, "Init Test", memoryBlockManagerInitTest) ||
        nullptr == CU_add_test(pSuite, "Init Zero Blocks Test", memoryBlockManagerInitZeroBlocksTest) ||
        nullptr == CU_add_test(pSuite, "Get Ptr Test", memoryBlockManagerGetPtrTest) ||
        nullptr == CU_add_test(pSuite, "Get Ptr Null Manager Test", memoryBlockManagerGetPtrNullManagerTest) ||
        nullptr == CU_add_test(pSuite, "Release Ptr Test", memoryBlockManagerReleasePtrTest) ||
        nullptr == CU_add_test(pSuite, "Release Ptr Bad Params Test", memoryBlockManagerReleasePtrBadParamsTest) ||
        nullptr == CU_add_test(pSuite, "Destroy Test", memoryBlockManagerDestroyTest) ||
        nullptr == CU_add_test(pSuite, "Destroy Null Test", memoryBlockManagerDestroyNullTest) ||
        nullptr == CU_add_test(pSuite, "Destroy With Active Blocks Test", memoryBlockManagerDestroyWithActiveBlocksTest) ||
        nullptr == CU_add_test(pSuite, "Multiple Alloc Release Test", memoryBlockManagerMultipleAllocReleaseTest))
    {
        CU_cleanup_registry();
        return;
    }
}
