
#include "pray_component.h"
#include "pray_default_components.h"

void prayRegisterDefaultComponents()
{
    registerComponent(Transform2D, nullptr, nullptr);
    registerComponent(Sprite2D, nullptr, nullptr);
}
