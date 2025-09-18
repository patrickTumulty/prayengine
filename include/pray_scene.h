
#ifndef PRAY_SCENE_H
#define PRAY_SCENE_H

#include "pray_system.h"

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
    char name[25];
    SceneCallbacks callbacks;
    SystemsContext systemsContext;
} Scene;

Scene *praySceneNew(char *name, SceneCallbacks callbacks);
void praySceneFree(Scene *scene);

#endif // PRAY_SCENE_H
