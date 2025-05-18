
#ifndef COMMON_HASH_H
#define COMMON_HASH_H

#include "common_types.h"
#include <stddef.h>

u32 hashFNV_1_32(const u8 *buf, size_t bufLen);
u32 hashFNV_1A_32(const u8 *buf, size_t bufLen);
u64 hashFNV_1_64(const u8 *buf, size_t bufLen);
u64 hashFNV_1A_64(const u8 *buf, size_t bufLen);

#endif // COMMON_HASH_H
