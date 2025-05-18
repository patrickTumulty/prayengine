
#include "common_hash.h"
#include "common_types.h"
#include <stddef.h>


static constexpr u32 FNV_OFFSET_BASIS_32 = 2166136261u;
static constexpr u64 FNV_OFFSET_BASIS_64 = 14695981039346656037ul;

static constexpr u32 FNV_PRIME_32 = 16777619u;
static constexpr u64 FNV_PRIME_64 = 1099511628211ul;

/**
 * @brief FNV-1 (Fowler-Noll-Vo) 
 *
 * Wikipedia: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 *
 * The FNV hash algorithm was designed for speed. It is intended to be used for hash tables and 
 * checksums. This algorithm is not suitable for cryptography. 
 *
 * @param[[in]] buf buffer to hash 
 * @param[[in]] bufLen buffer len 
 *
 * @return FNV-1a 32-bit hash
 */
u32 hashFNV_1_32(const u8 *buf, size_t bufLen)
{
    u32 hash = FNV_OFFSET_BASIS_32; 
    for (int i = 0; i < bufLen; i++)
    {
        hash *= FNV_PRIME_32;   
        hash ^= buf[i];
    }
    return hash;
}


/**
 * @brief FNV-1a (Fowler-Noll-Vo) 
 *
 * Wikipedia: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 *
 * FNV-1a is the preferred variant of the FNV algorithm
 *
 * The FNV hash algorithm was designed for speed. It is intended to be used for hash tables and 
 * checksums. This algorithm is not suitable for cryptography. 
 *
 * @param[[in]] buf buffer to hash 
 * @param[[in]] bufLen buffer len 
 *
 * @return FNV-1a 32-bit hash
 */
u32 hashFNV_1A_32(const u8 *buf, size_t bufLen)
{
    u32 hash = FNV_OFFSET_BASIS_32; 
    for (int i = 0; i < bufLen; i++)
    {
        hash ^= buf[i]; 
        hash *= FNV_PRIME_32;   
    }
    return hash;
}

/**
 * @brief FNV-1 (Fowler-Noll-Vo) 
 *
 * Wikipedia: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 *
 * The FNV hash algorithm was designed for speed. It is intended to be used for hash tables and 
 * checksums. This algorithm is not suitable for cryptography. 
 *
 * @param[[in]] buf buffer to hash 
 * @param[[in]] bufLen buffer len 
 *
 * @return FNV-1a 64-bit hash
 */
u64 hashFNV_1_64(const u8 *buf, size_t bufLen)
{
    u32 hash = FNV_OFFSET_BASIS_32; 
    for (int i = 0; i < bufLen; i++)
    {
        hash *= FNV_PRIME_32;   
        hash ^= buf[i];
    }
    return hash;
}


/**
 * @brief FNV-1a (Fowler-Noll-Vo) 
 *
 * Wikipedia: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 *
 * FNV-1a is the preferred variant of the FNV algorithm
 *
 * The FNV hash algorithm was designed for speed. It is intended to be used for hash tables and 
 * checksums. This algorithm is not suitable for cryptography. 
 *
 * @param[[in]] buf buffer to hash 
 * @param[[in]] bufLen buffer len 
 *
 * @return FNV-1a 64-bit hash
 */
u64 hashFNV_1A_64(const u8 *buf, size_t bufLen)
{
    u64 hash = FNV_OFFSET_BASIS_64; 
    for (int i = 0; i < bufLen; i++)
    {
        hash ^= buf[i]; 
        hash *= FNV_PRIME_64;   
    }
    return hash;
}
