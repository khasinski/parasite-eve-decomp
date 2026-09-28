#ifndef PE1_GLOBAL_POINTER_SLOT_H
#define PE1_GLOBAL_POINTER_SLOT_H

#include "common.h"

typedef struct GlobalPointerSlot {
    u8 *value;
    u8 pad[8];
} GlobalPointerSlot;

PE1_STATIC_ASSERT(sizeof(GlobalPointerSlot) == 0x0C,
                  global_pointer_slot_size);

#endif
