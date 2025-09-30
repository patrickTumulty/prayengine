
#include "common_tests.h"
#include "common_utils.h"
#include "tmem.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <float.h>
#include <stdint.h>


void commonUtilsMatrixAllocateTest(void)
{
    int height = 10;
    int width = 20;

    int **mat = (int **) allocateMatrix(height, width, sizeof(int));
    CU_ASSERT_PTR_NOT_NULL_FATAL(mat);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            mat[i][j] = i * j; 
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            CU_ASSERT_PTR_EQUAL_FATAL(mat[i][j], i * j);
        }
    }

    tmemfree((void *) mat);

    auto stats = tMemGetStats();
    CU_ASSERT_EQUAL(stats.current, 0);
}

void registerUtilsTests(void)
{
    CU_pSuite pSuite = nullptr;

    pSuite = CU_add_suite("Common Utils Tests", nullptr, nullptr);
    if (nullptr == pSuite)
    {
        CU_cleanup_registry();
        return;
    }

    if (nullptr == CU_add_test(pSuite, "Matrix Allocate Test", commonUtilsMatrixAllocateTest))
    {
        CU_cleanup_registry();
        return;
    }
}
