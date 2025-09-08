
#include "pray_default_systems.h"
#include "linked_list.h"
#include "pray_default_components.h"
#include "pray_entity.h"
#include "pray_entity_registry.h"
#include "pray_system.h"
#include "pray_utils.h"
#include "raylib.h"

void prayRenderShape2D(Entity *entity)
{
    auto shape2D = getComponent(entity, Shape2D);
    auto transform = getComponent(entity, Transform2D);

    float rotation = 0;
    Vector2 position = VEC2_ZERO;

    if (shape2D == nullptr)
    {
        return;
    }

    if (transform != nullptr)
    {
        position = transform->position;
        rotation = transform->rotation;
    }

    switch (shape2D->shapeType)
    {
        case SHAPE_TYPE_CIRCLE:
            DrawCircleV(position, shape2D->circle.radius, shape2D->color);
            break;
        case SHAPE_TYPE_RECTANGLE:
            Rectangle rec = {
                .x = 0,
                .y = 0,
                .height = shape2D->rectangle.height,
                .width = shape2D->rectangle.width,
            };
            DrawRectanglePro(rec, position, rotation, shape2D->color);
            break;
    }
}

void prayRenderSprite2D(Entity *entity)
{
    auto sprite2d = getComponent(entity, Sprite2D);
    auto transform = getComponent(entity, Transform2D);

    Vector2 position = {0, 0};
    float rotationDegrees = 0;

    if (sprite2d == nullptr)
    {
        return;
    }

    if (transform != nullptr)
    {
        position = transform->position;
        rotationDegrees = transform->rotation;
    }

    Rectangle source = sprite2d->source;

    if (sprite2d->shader != nullptr)
    {
        BeginShaderMode(*sprite2d->shader);

        sprite2d->preShaderCallback(entity, sprite2d);
    }

    DrawTexturePro(sprite2d->texture,
                   sprite2d->source,
                   (Rectangle) {position.x, position.y, source.width, source.height},
                   sprite2d->origin,
                   rotationDegrees + sprite2d->rotation,
                   WHITE);

    if (sprite2d->shader != nullptr)
    {
        EndShaderMode();
    }
}

static void renderUpdate()
{
    LList entities;
    Rc rc = prayEntityLookupAll(&entities, C(typeid(Sprite2D)), 1);
    if (rc != RC_OK)
    {
        return;
    }

    LNode *node = nullptr;
    LListForEach(&entities, node)
    {
        Entity *entity = LListGetEntry(node, Entity);
        prayRenderSprite2D(entity);
    }
}

static void registerSprite2DSystem(SystemsContext *context)
{
    System sprite2dSystem = {
        .name = "Sprite 2D System",
        .renderUpdateWorldSpace = renderUpdate,
    };
    praySystemsRegister(context, sprite2dSystem);
}

void prayRegisterDefaultSystems(SystemsContext *context)
{
    registerSprite2DSystem(context);
}
