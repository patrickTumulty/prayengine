
#ifndef PRAY_ENGINE_H
#define PRAY_ENGINE_H

#include "pray_scene.h"

void prayEngineLoadScene(Scene *scene, void *sceneParams);
void prayEngineRun();
void prayEngineStop();
void prayEngineInit(char *windowTitle);
void prayEngineDestroy();

#endif // PRAY_ENGINE_H
