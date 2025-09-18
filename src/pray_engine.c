
#include "pray_engine.h"
#include "logging_facade.h"
#include "pray_camera.h"
#include "pray_entity_registry.h"
#include "pray_globals.h"
#include "pray_scene.h"
#include "pray_system.h"
#include "raylib.h"
#include "tmem.h"
#include <inttypes.h>
#include <stdio.h>

typedef struct
{
    Scene *currentScene;
    void *sceneParams;
    Scene *nextScene;
    bool sceneLoopRunning;
    bool engineLoopRunning;
    bool engineStopRequested;
} PrayEngineContext;

static PrayEngineContext engineContext = {};

void prayEngineLoadScene(Scene *scene, void *sceneParams)
{
    lfLogInfo("Loading scene %s : params %p", scene->name, sceneParams);
    engineContext.sceneLoopRunning = false;
    engineContext.nextScene = scene;
    engineContext.sceneParams = sceneParams;
}

static void prayEngineRunScene(Scene *scene)
{
    if (scene == nullptr)
    {
        return;
    }

    lfLogInfo("*** Starting scene %s", scene->name);

    engineContext.sceneLoopRunning = true;

    scene->callbacks.init(&scene->systemsContext, engineContext.sceneParams);

    scene->callbacks.start(&scene->systemsContext);

    praySystemsRunInit(&scene->systemsContext);

    praySystemsRunStart(&scene->systemsContext);

    while (engineContext.sceneLoopRunning && !WindowShouldClose())
    {
        praySystemsRunGameUpdate(&scene->systemsContext);

        BeginDrawing();

        ClearBackground(DARKGRAY);

        BeginMode2D(*prayGetCamera());

        praySystemsRunRenderUpdateWorldSpace(&scene->systemsContext);

        EndMode2D();

        praySystemsRunRenderUpdateScreenSpace(&scene->systemsContext);

        EndDrawing();
    }

    praySystemsRunStop(&scene->systemsContext);

    praySystemsRunDestroy(&scene->systemsContext);

    scene->callbacks.stop(&scene->systemsContext);

    scene->callbacks.destroy(&scene->systemsContext);

    lfLogInfo("*** Exiting scene %s", scene->name);
}

void prayEngineRun()
{
    lfLogInfo("** Pray Engine Starting");

    engineContext.engineLoopRunning = true;

    while (engineContext.engineLoopRunning && !WindowShouldClose())
    {
        prayEngineRunScene(engineContext.currentScene);

        if (engineContext.nextScene != nullptr)
        {
            lfLogInfo("Changing to next scene");
            engineContext.currentScene = engineContext.nextScene;
            engineContext.nextScene = nullptr;
        }
    }

    CloseWindow();

    lfLogInfo("** Pray Engine Stopped");
}

void prayEngineStop()
{
    engineContext.engineStopRequested = true;
    engineContext.engineLoopRunning = false;
    engineContext.sceneLoopRunning = false;
}

void prayEngineInit(char *windowTitle)
{
    tMemInit();
    prayEntityRegistryInit();

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, windowTitle);

    SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MAXIMIZED);
    SetWindowMinSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    // SetExitKey(KEY_NULL);

    SetTargetFPS(60);
}

void prayEngineDestroy()
{
    prayEntityRegistryDestroy();
    tMemDestroy();
}
