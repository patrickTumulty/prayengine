
#include "pray_scene.h"
#include "pray_system.h"
#include "tmem.h"
#include <stdio.h>


static void initSceneNoOpCallback(SystemsContext *context, void *params)
{
    // No op
}

static void sceneNoOpCallback(SystemsContext *context)
{
    // No op
}

#define SET_IF_NULL(VAR, DEFAULT) \
    if ((VAR) == nullptr)         \
    {                             \
        (VAR) = (DEFAULT);        \
    }

Scene *praySceneNew(char *name, SceneCallbacks callbacks)
{
    Scene *scene = tmemcalloc(1, sizeof(Scene));
    scene->callbacks = callbacks;
    SET_IF_NULL(scene->callbacks.init, initSceneNoOpCallback);
    SET_IF_NULL(scene->callbacks.start, sceneNoOpCallback);
    SET_IF_NULL(scene->callbacks.stop, sceneNoOpCallback);
    SET_IF_NULL(scene->callbacks.destroy, sceneNoOpCallback);
    praySystemsContextInit(&scene->systemsContext);
    snprintf(scene->name, sizeof(scene->name), "%s", name);
    return scene;
}

void praySceneFree(Scene *scene)
{
    praySystemsContextDestroy(&scene->systemsContext);
    tmemfree(scene);
}
