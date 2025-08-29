
#include "common_tests.h"
#include "linked_list.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>


typedef struct
{
    int num;
    LNode node;
} NumNode;

void verifyContents(LList *list, const int *numArray, int numArrayLen)
{
    LNode *node = nullptr;
    int count = 0;

    CU_ASSERT_EQUAL(list->size, numArrayLen);

    LListForEach(list, node)
    {
        NumNode *num = LListGetEntry(node, NumNode);
        CU_ASSERT_PTR_NOT_NULL_FATAL(num);
        CU_ASSERT_EQUAL(num->num, numArray[count++]);
    }
}

void llistAddingElementsTest(void)
{
    LList list = {};
    llistInit(&list);

    const int LIST_SIZE = 10;

    NumNode nodes[LIST_SIZE];
    for (int i = 0; i < LIST_SIZE - 1; i++)
    {
        NumNode *n = &nodes[i];
        n->num = i;
        llistInitNode(&n->node, n);
        llistAppend(&list, &n->node);
    }

    CU_ASSERT_EQUAL(list.size, LIST_SIZE - 1);

    LNode *n = llistRemove(&list, &nodes[LIST_SIZE - 1].node);
    CU_ASSERT_PTR_NULL(n); // This node wasn't added, so removal returns null

    LNode *node = nullptr;
    int count = 0;
    LListForEach(&list, node)
    {
        NumNode *n = LListGetEntry(node, NumNode);
        CU_ASSERT_PTR_NOT_NULL_FATAL(n);
        CU_ASSERT_EQUAL(n->num, count++);
    }

    CU_ASSERT_EQUAL(count, LIST_SIZE - 1);

    n = llistRemove(&list, &nodes[3].node);
    CU_ASSERT_PTR_NOT_NULL(n);

    n = llistRemove(&list, &nodes[0].node);
    CU_ASSERT_PTR_NOT_NULL(n);

    int contents[LIST_SIZE];
    memset(contents, 0, sizeof(contents));
    count = 0;
    LListForEach(&list, node)
    {
        NumNode *n = LListGetEntry(node, NumNode);
        CU_ASSERT_PTR_NOT_NULL_FATAL(n);
        contents[count++] = n->num;
    }

    int expected[] = {1, 2, 4, 5, 6, 7, 8, 0, 0, 0};
    for (int i = 0; i < LIST_SIZE; i++)
    {
        CU_ASSERT_EQUAL(contents[i], expected[i]);
    }
}

void llistPushPopTest(void)
{
    LList list = {};
    llistInit(&list);

    const int LIST_SIZE = 10;

    NumNode nodes[LIST_SIZE];
    for (int i = 0; i < LIST_SIZE; i++)
    {
        NumNode *n = &nodes[i];
        n->num = i;
        llistInitNode(&n->node, n);
    }

    llistPushBack(&list, &nodes[1].node);
    CU_ASSERT_EQUAL(list.size, 1);
    llistPushFront(&list, &nodes[4].node);
    CU_ASSERT_EQUAL(list.size, 2);
    llistPushBack(&list, &nodes[2].node);
    CU_ASSERT_EQUAL(list.size, 3);
    llistPushBack(&list, &nodes[8].node);
    CU_ASSERT_EQUAL(list.size, 4);
    llistPushFront(&list, &nodes[3].node);
    CU_ASSERT_EQUAL(list.size, 5);

    int expected1[] = {3, 4, 1, 2, 8};
    verifyContents(&list, expected1, 5);

    LNode *n = nullptr;
    n = llistPopBack(&list);
    CU_ASSERT_PTR_NOT_NULL(n)
    CU_ASSERT_EQUAL(n, &nodes[8].node);

    n = llistPopBack(&list);
    CU_ASSERT_PTR_NOT_NULL(n)
    CU_ASSERT_EQUAL(n, &nodes[2].node);

    n = llistPopFront(&list);
    CU_ASSERT_PTR_NOT_NULL(n)
    CU_ASSERT_EQUAL(n, &nodes[3].node);

    int expected2[] = {4, 1};
    verifyContents(&list, expected2, 2);

    n = llistPopFront(&list);
    CU_ASSERT_PTR_NOT_NULL(n)
    CU_ASSERT_EQUAL(n, &nodes[4].node);

    n = llistPopFront(&list);
    CU_ASSERT_PTR_NOT_NULL(n)
    CU_ASSERT_EQUAL(n, &nodes[1].node);

    CU_ASSERT_EQUAL(list.size, 0);

    n = llistPopFront(&list);
    CU_ASSERT_PTR_NULL(n)

    CU_ASSERT_EQUAL(list.size, 0);
}

void llistPopFrontTillEmpty()
{
    LList list = {};
    llistInit(&list);

    const int LIST_SIZE = 5;

    NumNode nodes[LIST_SIZE];
    for (int i = 0; i < LIST_SIZE; i++)
    {
        NumNode *n = &nodes[i];
        n->num = i;
        llistInitNode(&n->node, n);
        llistAppend(&list, &n->node);
    }

    LNode *n = nullptr;
    for (int i = 0; i < LIST_SIZE; i++)
    {
        n = llistPopFront(&list);
        CU_ASSERT_PTR_NOT_NULL(n)
        CU_ASSERT_EQUAL(n, &nodes[i].node);
        CU_ASSERT_EQUAL(list.size, LIST_SIZE - i - 1);
    }

    n = llistPopFront(&list);
    CU_ASSERT_PTR_NULL(n)
    CU_ASSERT_EQUAL(list.size, 0);

    n = llistPopFront(&list);
    CU_ASSERT_PTR_NULL(n)
    CU_ASSERT_EQUAL(list.size, 0);
}

void llistPopBackTillEmpty()
{
    LList list = {};
    llistInit(&list);

    const int LIST_SIZE = 5;

    NumNode nodes[LIST_SIZE];
    for (int i = 0; i < LIST_SIZE; i++)
    {
        NumNode *n = &nodes[i];
        n->num = i;
        llistInitNode(&n->node, n);
        llistAppend(&list, &n->node);
    }

    LNode *n = nullptr;
    for (int i = 0; i < LIST_SIZE; i++)
    {
        n = llistPopBack(&list);
        CU_ASSERT_PTR_NOT_NULL(n)
        CU_ASSERT_EQUAL(n, &nodes[LIST_SIZE - i - 1].node);
        CU_ASSERT_EQUAL(list.size, LIST_SIZE - i - 1);
    }

    n = llistPopBack(&list);
    CU_ASSERT_PTR_NULL(n)
    CU_ASSERT_EQUAL(list.size, 0);

    n = llistPopBack(&list);
    CU_ASSERT_PTR_NULL(n)
    CU_ASSERT_EQUAL(list.size, 0);
}

void llistTransferBetweeenQueues()
{
    LList inList = {};
    llistInit(&inList);
    LList outList = {};
    llistInit(&outList);

    const int LIST_SIZE = 2;

    NumNode nodes[LIST_SIZE];
    for (int i = 0; i < LIST_SIZE; i++)
    {
        NumNode *n = &nodes[i];
        n->num = i;
        llistInitNode(&n->node, n);
        llistAppend(&inList, &n->node);
    }

    CU_ASSERT_EQUAL(inList.size, 2);
    LNode *n0 = llistPopFront(&inList);
    CU_ASSERT_PTR_NOT_NULL(n0);
    CU_ASSERT_EQUAL(inList.size, 1);
    CU_ASSERT_PTR_NULL(n0->next);
    CU_ASSERT_PTR_NULL(n0->prev);


    CU_ASSERT_EQUAL(inList.size, 1);
    LNode *n1 = llistPopFront(&inList);
    CU_ASSERT_PTR_NOT_NULL(n1);
    CU_ASSERT_EQUAL(inList.size, 0);
    CU_ASSERT_PTR_NULL(n1->next);
    CU_ASSERT_PTR_NULL(n1->prev);

    CU_ASSERT_EQUAL(outList.size, 0);
    llistAppend(&outList, n0);
    CU_ASSERT_EQUAL(outList.size, 1);

    LNode *lnode = nullptr;
    int inListCount = 0;
    int inListSize = (int) inList.size;
    int outListCount = 0;
    int outListSize = (int) outList.size;

    while ((lnode = llistPopFront(&inList)) != nullptr)
    {
        inListCount++;
    }

    while ((lnode = llistPopFront(&outList)) != nullptr)
    {
        outListCount++;
    }

    CU_ASSERT_EQUAL(inListCount, inListSize);
    CU_ASSERT_EQUAL(outListCount, outListSize);
}

void registerLinkedListTests(void)
{
    CU_pSuite suite = CU_add_suite("Linked List Tests", nullptr, nullptr);
    CU_add_test(suite, "Adding/Removing Elements", llistAddingElementsTest);
    CU_add_test(suite, "Push/Pop Elements", llistPushPopTest);
    CU_add_test(suite, "Pop Till Front Empty", llistPopFrontTillEmpty);
    CU_add_test(suite, "Pop Till Back Empty", llistPopBackTillEmpty);
    CU_add_test(suite, "Transfer Between Queues", llistTransferBetweeenQueues);
}
