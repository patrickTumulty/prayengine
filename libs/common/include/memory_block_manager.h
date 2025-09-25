
#ifndef MEMORY_BLOCK_MANAGER_H
#define MEMORY_BLOCK_MANAGER_H

#include "common_types.h"

typedef struct {
    void *internal;
} MemoryBlockManager;

Rc memoryBlockManagerInit(MemoryBlockManager *manager, u32 blockSize, u32 initialBlockCount);
Rc memoryBlockManagerDestroy(MemoryBlockManager *manager, bool force);
void *memoryBlockManagerGetPtr(MemoryBlockManager *manager);
Rc memoryBlockManagerReleasePtr(MemoryBlockManager *manager, void *ptr);

#endif // MEMORY_BLOCK_MANAGER_H
