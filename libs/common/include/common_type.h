
#ifndef COMMON_TYPE_H
#define COMMON_TYPE_H

#include "common_hash.h"
#include "common_types.h"
#include <string.h>

#define TYPE_NAME_LEN 50

typedef u32 type_id;

typedef struct type
{
    char name[TYPE_NAME_LEN];
    type_id id;
} type_t;

#define type(T) (__type_##T)
#define typeid(T) (type(T).id)
#define typename(T) ((type(T).name)

/*
 * Step 1: Use DECLARE_TYPE in header file. 
 *
 * Example: test.h
 *
 * typedef struct {
 *     int a;
 * } MyStruct;
 *
 * DECLARE_TYPE(MyStruct);
 */
#define DECLARE_TYPE(T) extern type_t type(T);

/*
 * Step 2: Use DEFINE_TYPE in cooresponding source file
 *
 * Example: test.c
 *
 * DEFINE_TYPE(MyStruct);
 */
#define DEFINE_TYPE(T) type_t type(T) = {.name = #T, .id = 0};

/*
 * Step 3: Use REGISTER_TYPE in a function that gets called at runtime. 
 *
 * Example: main.c
 *
 * int main() {
 *     REGISTER_TYPE(MyStruct);
 * }
 */
#define REGISTER_TYPE(T) (type(T).id = hashFNV_1A_32((u8 *) type(T).name, strlen(type(T).name)));

#endif // COMMON_TYPE_H
