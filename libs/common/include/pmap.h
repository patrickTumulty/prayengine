
#ifndef PMAP_H
#define PMAP_H


#include "common_types.h"

// TODO(ptumulty): implement all this stuff

typedef struct {
    u64 key;
    void *value;
} PMapEntry;

typedef struct {
    float loadFactor;
    u32 entriesLen;
    PMapEntry* entries;
} PMap;

Rc pmapNew(PMap *pmap);
Rc pmapPut(PMap *pmap, u32 key, void *value);
void *pmapGet(PMap *pmap, u32 key);
void *pmapRemove(PMap *pmap, u32 key);
Rc pmapClear(PMap *pmap);
Rc pmapFree(PMap *pmap);

#endif // PMAP_H
