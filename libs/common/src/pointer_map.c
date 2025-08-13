
#include "pointer_map.h"
#include "common_types.h"
#include "common_utils.h"
#include "tmem.h"
#include <stdio.h>

#define PMAP_CRITICAL_LOAD_FACTOR (0.56f)
#define PMAP_RESIZE_INCREMENT 5
#define PMAP_INITIAL_MAP_SIZE 16

typedef struct
{
    u32 key;
    void *value;
    bool isSet;
} PMapEntry;

typedef struct
{
    PMapEntry *entries;
    u32 entriesLen;
    float loadFactor;
} PMapInternal;

#define getInternal(PMAP) ((PMapInternal *) ((PMAP)->internal))

static void pmapCalcLoadFactor(PMapInternal *internal)
{
    u32 count = 0;
    for (int i = 0; i < internal->entriesLen; i++)
    {
        if (internal->entries[i].isSet)
        {
            count++;
        }
    }
    internal->loadFactor = (float) count / (float) internal->entriesLen;
}

// static void printEntries(PMapInternal *internal)
// {
//     printf("lf=%f\n", internal->loadFactor);
//     for (int i = 0; i < internal->entriesLen; i++)
//     {
//         PMapEntry *entry = &internal->entries[i];
//         printf(" - %2d  %X : %p%s\n", i, entry->key, entry->value, entry->isSet ? " (set)" : "");
//     }
// }

static Rc findUnsetEntry(PMapInternal *internal, u32 key, PMapEntry **entryOut)
{
    u32 counter = 0;
    u32 insertIndex = key % internal->entriesLen;
    do {
        PMapEntry *entry = &internal->entries[insertIndex];
        if (!entry->isSet)
        {
            *entryOut = entry;
            return RC_OK;
        }
        if (entry->isSet && entry->key == key)
        {
            return RC_NOT_ALLOWED; // duplicate key
        }
        insertIndex = (insertIndex + 1) % internal->entriesLen;
        counter++;
    } while (counter < internal->entriesLen);
    return RC_NOT_FOUND;
}

Rc pmapNew(PMap *pmap)
{
    PMapInternal *internal = tmemcalloc(1, sizeof(PMapInternal));
    if (internal == nullptr)
    {
        return RC_MEM_ALLOC_ERROR;
    }
    pmap->internal = internal;
    internal->entries = tmemcalloc(1, sizeof(PMapEntry) * PMAP_INITIAL_MAP_SIZE);
    if (internal->entries == nullptr)
    {
        tmemfree(internal);
        return RC_MEM_ALLOC_ERROR;
    }
    internal->entriesLen = PMAP_INITIAL_MAP_SIZE;
    pmap->size = 0;
    pmapCalcLoadFactor(internal);
    return RC_OK;
}

static Rc pmapPutInternal(PMapInternal *internal, u32 key, void *value)
{
    PMapEntry *entry = nullptr;
    Rc rc = findUnsetEntry(internal, key, &entry);
    if (entry == nullptr || rc != RC_OK)
    {
        return RC_BAD_PARAM;
    }
    entry->key = key;
    entry->value = value;
    entry->isSet = true;
    pmapCalcLoadFactor(internal);
    return RC_OK;
}

static Rc pmapResize(PMapInternal *internal, u32 increase)
{
    PMapEntry *newEntries = tmemcalloc(1, sizeof(PMapEntry) * (internal->entriesLen + increase));
    if (newEntries == nullptr)
    {
        return RC_MEM_ALLOC_ERROR;
    }
    PMapEntry *existingEntries = internal->entries;
    u32 existingEntriesLen = internal->entriesLen;
    internal->entries = newEntries;
    internal->entriesLen += increase;
    for (int i = 0; i < existingEntriesLen; i++)
    {
        PMapEntry *entry = &existingEntries[i];
        if (!entry->isSet)
        {
            continue;
        }
        Rc rc = pmapPutInternal(internal, entry->key, entry->value);
        if (rc != RC_OK)
        {
            /*
             * Something went wrong. Revert 
             * the map to the existing state. 
             */
            internal->entries = existingEntries;
            internal->entriesLen = existingEntriesLen;
            tmemfree(newEntries);
            return rc;
        }
    }
    tmemfree(existingEntries);
    pmapCalcLoadFactor(internal);
    return RC_OK;
}

static PMapEntry *findEntry(PMapInternal *internal, u32 key)
{
    u32 counter = 0;
    u32 insertIndex = key % internal->entriesLen;
    do {
        PMapEntry *entry = &internal->entries[insertIndex];
        if (entry->isSet && entry->key == key)
        {
            return entry;
        }
        insertIndex = (insertIndex + 1) % internal->entriesLen;
        counter++;
    } while (counter < internal->entriesLen);
    return nullptr;
}


Rc pmapPut(PMap *pmap, u32 key, void *value)
{
    Rc rc = RC_OK;
    PMapInternal *internal = getInternal(pmap);
    if (internal->loadFactor >= PMAP_CRITICAL_LOAD_FACTOR)
    {
        rc = pmapResize(internal, PMAP_RESIZE_INCREMENT);
        if (rc != RC_OK)
        {
            return rc;
        }
    }
    rc = pmapPutInternal(internal, key, value);
    if (rc == RC_OK)
    {
        pmap->size += 1;
    }
    return rc;
}

void *pmapGet(PMap *pmap, u32 key)
{
    PMapInternal *internal = getInternal(pmap);
    PMapEntry *entry = findEntry(internal, key);
    if (entry == nullptr)
    {
        return nullptr;
    }
    return entry->value;
}

void *pmapRemove(PMap *pmap, u32 key)
{
    PMapInternal *internal = getInternal(pmap);
    PMapEntry *entry = findEntry(internal, key);
    if (entry == nullptr)
    {
        return nullptr;
    }
    entry->isSet = false;
    entry->key = 0;
    void *value = entry->value;
    entry->value = nullptr;
    pmapCalcLoadFactor(internal);
    pmap->size--;
    return value;
}

Rc pmapClear(PMap *pmap)
{
    PMapInternal *internal = getInternal(pmap);
    for (int i = 0; i < internal->entriesLen; i++)
    {
        PMapEntry *entry = &internal->entries[i];
        entry->isSet = false;
        entry->key = 0;
        entry->value = nullptr;
    }
    pmapCalcLoadFactor(internal);
    return RC_OK;
}

Rc pmapFree(PMap *pmap)
{
    PMapInternal *internal = getInternal(pmap);
    tmemfree(internal->entries);
    tmemfree(internal);
    return RC_OK;
}
