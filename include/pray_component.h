
#ifndef PRAY_COMPONENTS_H
#define PRAY_COMPONENTS_H

#include "common_type.h"
#include "common_types.h"
#include "linked_list.h"
#include <raylib.h>

#define C(...) \
    (type_id[]) { __VA_ARGS__ }

typedef struct
{
    type_id id;
    u64 size;
    Consumer initialize;
    Consumer deinitialize;
    LNode lnode;
} ComponentInitializer;

Rc prayComponentGetInitializer(type_id componentID, ComponentInitializer *componentInitialzer);
Rc prayComponentRegister(type_id componentID,
                         u64 componentSize,
                         Consumer initCallback,
                         Consumer deinitCallback);
void prayComponnentInitialize();
void prayComponentsDestroy();

#define registerComponent(TYPE, INIT, DEINIT) prayComponentRegister(typeid(TYPE), sizeof(TYPE), INIT, DEINIT);

#endif // PRAY_COMPONENTS_H
