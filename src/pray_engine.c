
#include "pray_engine.h"
#include "pray_camera.h"
#include "pray_entity_registry.h"
#include "pray_globals.h"
#include "pray_scene.h"
#include "pray_system.h"
#include "raylib.h"
#include "tmem.h"

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
}

void prayEngineRun()
{
    engineContext.engineLoopRunning = true;

    while (engineContext.engineLoopRunning && !WindowShouldClose())
    {
        prayEngineRunScene(engineContext.currentScene);

        if (engineContext.nextScene != nullptr)
        {
            engineContext.currentScene = engineContext.nextScene;
            engineContext.nextScene = nullptr;
        }
    }

    CloseWindow();
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
