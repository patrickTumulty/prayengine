
#ifndef PRAY_DEFAULT_COMPONENTS_H
#define PRAY_DEFAULT_COMPONENTS_H

#include "common_type.h"
#include "pray_component.h"
#include "pray_entity.h"
#include "raylib.h"

typedef struct
{
    Vector2 position;
    float rotation; // Degrees (0.0 to 359.0)
} Transform2D;

DECLARE_TYPE(Transform2D);

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

DECLARE_TYPE(Sprite2D);

void prayRenderSprite2D(Entity *entity);
void prayRegisterDefaultComponents();

#endif // PRAY_COMPONENTS_H
