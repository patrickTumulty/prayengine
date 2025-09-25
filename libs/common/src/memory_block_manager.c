
#include "memory_block_manager.h"
#include "common_types.h"
#include "linked_list.h"
#include "tmem.h"
#include <assert.h>

static constexpr u32 MEM_BLOCK_MAGIC_NUMBER = 0xDEADCAFE;

typedef struct
{
    u32 magic;
    u32 blockSize;
    void *data;
    LNode node;
} MemoryBlock;

typedef struct
{
    LList activeMemoryBlocks;
    LList freeMemoryBlocks;
    u32 blockSize;
} MemoryBlockManagerInternal;

static MemoryBlock *memoryBlockAllocateNewBlock(u32 blockSize)
{
    void *ptr = tmemcalloc(1, sizeof(MemoryBlock) + blockSize);
    if (ptr == nullptr)
    {
        return nullptr;
    }

    MemoryBlock *memoryBlock = ptr;
    llistInitNode(&memoryBlock->node, memoryBlock);

    ptr += sizeof(MemoryBlock);

    memoryBlock->magic = MEM_BLOCK_MAGIC_NUMBER;
    memoryBlock->blockSize = blockSize;
    memoryBlock->data = ptr;

    return memoryBlock;
}

Rc memoryBlockManagerInit(MemoryBlockManager *manager, u32 blockSize, u32 initialBlockCount)
{
    MemoryBlockManagerInternal *internal = tmemcalloc(1, sizeof(MemoryBlockManagerInternal));
    if (internal == nullptr)
    {
        return RC_MEM_ALLOC_ERROR;
    }
    internal->activeMemoryBlocks = LListStaticInit();
    internal->freeMemoryBlocks = LListStaticInit();
    internal->blockSize = blockSize;
    manager->internal = internal;

    for (int i = 0; i < initialBlockCount; i++)
    {
        MemoryBlock *memoryBlock = memoryBlockAllocateNewBlock(internal->blockSize);
        if (memoryBlock == nullptr)
        {
            continue;
        }
        llistAppend(&internal->freeMemoryBlocks, &memoryBlock->node);
    }

    return RC_OK;
}

Rc memoryBlockManagerDestroy(MemoryBlockManager *manager, bool force)
{
    if (manager == nullptr)
    {
        return RC_BAD_PARAM;
    }

    MemoryBlockManagerInternal *internal = manager->internal;

    if (internal->activeMemoryBlocks.size != 0 && !force)
    {
        return RC_NOT_ALLOWED; // Still have active memory blocks
    }

    LNode *node = nullptr;

    while ((node = llistPopFront(&internal->activeMemoryBlocks)) != nullptr)
    {
        MemoryBlock *memoryBlock = LListGetEntry(node, MemoryBlock);
        tmemfree(memoryBlock);
    }

    while ((node = llistPopFront(&internal->freeMemoryBlocks)) != nullptr)
    {
        MemoryBlock *memoryBlock = LListGetEntry(node, MemoryBlock);
        tmemfree(memoryBlock);
    }

    tmemfree(internal);

    manager->internal = nullptr;

    return RC_OK;
}

void *memoryBlockManagerGetPtr(MemoryBlockManager *manager)
{
    if (manager == nullptr)
    {
        return nullptr;
    }

    MemoryBlockManagerInternal *internal = manager->internal;

    MemoryBlock *memoryBlock = nullptr;
    LNode *node = llistPopFront(&internal->freeMemoryBlocks);
    if (node == nullptr)
    {
        memoryBlock = memoryBlockAllocateNewBlock(internal->blockSize);
        if (memoryBlock == nullptr)
        {
            return nullptr;
        }
    }
    else
    {
        memoryBlock = LListGetEntry(node, MemoryBlock);
    }

    llistAppend(&internal->activeMemoryBlocks, &memoryBlock->node);

    return memoryBlock->data;
}

Rc memoryBlockManagerReleasePtr(MemoryBlockManager *manager, void *ptr)
{
    if (ptr == nullptr || manager == nullptr)
    {
        return RC_BAD_PARAM;
    }

    MemoryBlockManagerInternal *internal = manager->internal;

    void *memoryBlockPtr = ptr - sizeof(MemoryBlock);
    u32 *magic = (u32 *) memoryBlockPtr;
    u32 *blockSize = (u32 *) (memoryBlockPtr + sizeof(u32));
    assert(*magic == MEM_BLOCK_MAGIC_NUMBER && *blockSize == internal->blockSize);

    MemoryBlock *memoryBlock = memoryBlockPtr;

    LNode *node = llistRemove(&internal->activeMemoryBlocks, &memoryBlock->node);
    assert(node != nullptr); // Something is up, this block wasn't active
    llistAppend(&internal->freeMemoryBlocks, &memoryBlock->node);

    return RC_OK;
}
