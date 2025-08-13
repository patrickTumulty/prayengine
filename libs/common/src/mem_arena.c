
#include "mem_arena.h"
#include "common_types.h"
#include "tmem.h"

typedef struct
{
    u64 totalSize;
    u64 availableSize;
    u64 nextAvailable;
} ArenaHeader;

typedef struct
{
    u32 magic;
    u32 freed : 1, reserved : 31;
    u64 size;
    u8 data[];
} MemBlockHeader;

static constexpr u32 MEM_BLOCK_MAGIC = 0xDEADCAFE;

#define NULL_ARENA_BLOCK (ArenaBlock){.ptr = nullptr, .size = 0}

void mArenaNew(Arena *arena, u64 size)
{
    arena->data = tmemcalloc(1, sizeof(size));
    if (arena == nullptr)
    {
        return;
    }
    ArenaHeader *header = (ArenaHeader *) arena->data;
    header->totalSize = size;
    header->availableSize = size;
    header->nextAvailable = (u64) (arena->data + sizeof(ArenaHeader));
}


ArenaBlock mArenaAlloc(Arena *arena, u64 size)
{
    ArenaHeader *header = (ArenaHeader *) arena->data;

    if ((header->availableSize - sizeof(MemBlockHeader)) < size)
    {
        return NULL_ARENA_BLOCK;
    }


    MemBlockHeader *memBlockHeader = (MemBlockHeader *) header->nextAvailable;
    memBlockHeader->magic = MEM_BLOCK_MAGIC;
    memBlockHeader->freed = 0;
    memBlockHeader->size = size;


    u64 blockSize = size + sizeof(MemBlockHeader);
    if (blockSize < header->totalSize)
    {
        header->nextAvailable = (u64) (((void *) header->nextAvailable) + blockSize);
    }
    else
    {
        header->nextAvailable = 0;
    }
}
