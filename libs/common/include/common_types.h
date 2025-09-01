
#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <stdint.h>

typedef struct
{
    int x;
    int y;
} Position;

typedef struct
{
    int x;
    int y;
} Point2;

#define POINT2(X, Y) ((Point2) {.x = (X), .y = (Y)})
#define POINT2_EQ(P1, P2) (((P1).x == (P2).x) && ((P1).y == (P2).y))

typedef struct
{
    int x;
    int y;
    int z;
} Point3;

#define POINT3(X, Y, Z) ((Point3) {.x = (X), .y = (Y), .z = (Z)})
#define PIONT3_EQ(P1, P2) (((P1).x == (P2).x) && ((P1).y == (P2).y) && ((P1).z == (P2).z))

typedef enum
{
    RC_OK = 0,
    RC_ERROR = -1,
    RC_MEM_ALLOC_ERROR = -2,
    RC_BAD_PARAM = -3,
    RC_OUT_OF_BOUNDS = -4,
    RC_NOT_FOUND = -5,
    RC_NOT_ALLOWED = -6,
} Rc;

#define RETURN_IF_ERROR(RC) \
    if (rc != RC_OK) return rc

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef void (*Consumer)(void *);

#endif
