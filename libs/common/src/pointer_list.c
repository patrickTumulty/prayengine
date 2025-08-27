
#include "pointer_list.h"
#include "array_list.h"
#include "common_types.h"
#include <stdio.h>


Rc plistNew(PList *plist, u32 length)
{
    return alistNew(plist, length, sizeof(u64));
}

Rc plistAppend(PList *plist, const void *ptr)
{
    u64 temp = (u64) ptr;
    return alistAppend(plist, &temp);
}

Rc plistResize(PList *plist, u32 length)
{
    return alistResize(plist, length);
}

void *plistGet(PList *plist, int index)
{
    u64 *ptr = alistGet(plist, index);
    if (ptr == nullptr)
    {
        return nullptr;
    }
    return (void *) *ptr;
}

Rc plistSet(PList *plist, int index, const void *ptr)
{
    u64 temp = (u64) ptr;
    return alistSet(plist, index, &temp);
}

u32 plistLength(PList *plist)
{
    return alistLength(plist);
}

void plistFree(PList *plist)
{
    return alistFree(plist);
}

/**
 * @brief remove all nullptr elements and resize the array
 *
 * @param[[in,out]] plist pointer list 
 * @return RC_OK if successful  
 */
Rc plistReduce(PList *plist)
{
    if (plist->length == 0)
    {
        return RC_OK;
    }
    int i = 0;
    int end = (int) (plist->length - 1);
    while (i < plist->length)
    {
        void *ptr = plistGet(plist, i);
        if (ptr == nullptr && i < end)
        {
            plistSet(plist, i, plistGet(plist, end));
            end--;
        }
        else
        {
            i++;
        }
    }
    return plistResize(plist, end + 1);
}
