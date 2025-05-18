
#include "common_type.h"
#include "pray_component.h"
#include "common_hash.h"
#include "pray_default_components.h"

DEFINE_TYPE(Transform2D);
DEFINE_TYPE(Sprite2D);

void prayRegisterDefaultComponents()
{
    REGISTER_TYPE(Transform2D);
    registerComponent(Transform2D, nullptr, nullptr);

    REGISTER_TYPE(Sprite2D);
    registerComponent(Sprite2D, nullptr, nullptr);
}
