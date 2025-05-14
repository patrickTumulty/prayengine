
#include "pray_default_systems.h"
#include "linked_list.h"
#include "pray_default_components.h"
#include "pray_entity.h"
#include "pray_entity_registry.h"
#include "pray_system.h"

static void renderUpdate()
{
    LList entities;
    Rc rc = prayEntityLookupAll(&entities, C(CID(Sprite2D)), 1);
    if (rc != RC_OK)
    {
        return;
    }

    LNode *node = nullptr;
    LListForEach(&entities, node)
    {
        Entity *entity = LListGetEntry(node, Entity);
        auto sprite2d = getComponent(entity, Sprite2D);
        auto transform = getComponent(entity, Transform2D);

        Vector2 position = {0, 0};
        float rotationDegrees = 0;

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
}

static void registerSprite2DSystem()
{
    System sprite2dSystem = {
        .name = "Sprite 2D System",
        .renderUpdateWorldSpace = renderUpdate,
    };
    praySystemsRegister(sprite2dSystem);
}

void prayRegisterDefaultSystems()
{
    registerSprite2DSystem();
}
