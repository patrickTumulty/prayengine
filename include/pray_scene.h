
#ifndef PRAY_SCENE_H
#define PRAY_SCENE_H

#include "pray_system.h"
#include "raylib.h"

typedef void (*InitSceneCallback)(SystemsContext *systemsContext, void *params);
typedef void (*SceneCallback)(SystemsContext *systemsContext);

typedef struct {
    InitSceneCallback init;
    SceneCallback start;
    SceneCallback stop;
    SceneCallback destroy;
} SceneCallbacks;

typedef struct
{
    SceneCallbacks callbacks;
    SystemsContext systemsContext;
} Scene;

Scene *praySceneNew(SceneCallbacks callbacks);
void praySceneFree(Scene *scene);

#endif // PRAY_SCENE_H
