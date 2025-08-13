
#ifndef MEM_ARENA_H
#define MEM_ARENA_H

#include "common_types.h"

typedef struct {
    u8 *data;
} Arena;

typedef struct {
    void *ptr;
    u64 size;
} ArenaBlock;

void mArenaNew(Arena *arena, u64 size);
ArenaBlock mArenaAlloc(Arena *arena, u64 size);
void mArenaFree(Arena *arena, ArenaBlock arenaBlock);

#endif // MEM_ARENA_H
