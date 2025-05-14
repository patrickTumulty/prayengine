
#ifndef PRAY_DEFAULT_COMPONENTS_H
#define PRAY_DEFAULT_COMPONENTS_H

#include "pray_component.h"
#include "pray_entity.h"
#include "raylib.h"

#undef COMPONENT_OFFSET
#define COMPONENT_OFFSET COMPONENT_BANK_DEFAULT

typedef struct
{
    Vector2 position;
    float rotation; // Degrees (0.0 to 359.0)
} Transform2D;

REGISTER_CID(Transform2D);

constexpr char str[] = "Hello";

struct Sprite2D;

typedef void (*PreShaderCallback)(Entity *entity, struct Sprite2D *sprite2D);

typedef struct Sprite2D
{
    Texture2D texture;
    Rectangle source;
    Vector2 origin;
    float rotation; // degrees (0.0 to 359.0)
    Shader *shader;
    PreShaderCallback preShaderCallback;
} Sprite2D;

REGISTER_CID(Sprite2D);

void prayRegisterDefaultComponents();

#endif // PRAY_COMPONENTS_H
