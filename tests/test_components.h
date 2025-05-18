
#ifndef TEST_COMPONENTS_H
#define TEST_COMPONENTS_H

#include "common_type.h"
#include "common_types.h"
#include "raylib.h"

#define COMPONENT_OFFSET COMPONENT_BANK_A

typedef struct
{
    char playerName[20];
    int playerAge;
    char origin[20];
} PlayerComponent;

DECLARE_TYPE(PlayerComponent);

typedef struct
{
    char enemyName[20];
    int enemyAge;
    char origin[20];
} EnemyComponent;

DECLARE_TYPE(EnemyComponent);

typedef struct
{
    Vector2 position;
} TransformComponent;

DECLARE_TYPE(TransformComponent);

typedef struct
{
    u32 currentHealth;
    u32 maxHealth;
} HealthComponent;

DECLARE_TYPE(HealthComponent);

typedef struct
{
    u8 *world;
    u64 worldSize;
} WorldComponent;

DECLARE_TYPE(WorldComponent);

void registerTestComponents();


#endif // TEST_COMPONENTS_H
