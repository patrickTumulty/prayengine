
#ifndef POINTER_MAP_H
#define POINTER_MAP_H

#include "common_types.h"


typedef struct {
    u32 size;
    void *internal;
} PMap;

Rc pmapNew(PMap *pmap);
Rc pmapPut(PMap *pmap, u32 key, void *value);
void *pmapGet(PMap *pmap, u32 key);
void *pmapRemove(PMap *pmap, u32 key);
bool pmapContains(PMap *pmap, u32 key);
Rc pmapClear(PMap *pmap);
Rc pmapFree(PMap *pmap);
Rc pmapGetKeys(PMap *pmap, u32 *keysArray, u32 keysArrayLen);

#endif // POINTER_MAP_H
