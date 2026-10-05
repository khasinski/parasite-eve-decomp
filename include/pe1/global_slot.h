#ifndef PE1_GLOBAL_SLOT_H
#define PE1_GLOBAL_SLOT_H

#include "common.h"

typedef union Pe1GlobalSlotValue {
    u8 *pointer;
    s32 signed_value;
    u32 unsigned_value;
} Pe1GlobalSlotValue;

typedef struct Pe1GlobalSlot {
    /* 0x00 */ Pe1GlobalSlotValue value;
    /* 0x04 */ u8 tail[8];
} Pe1GlobalSlot;

PE1_STATIC_ASSERT(sizeof(Pe1GlobalSlotValue) == 4, pe1_global_slot_value_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GlobalSlot, value) == 0,
                  pe1_global_slot_value_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GlobalSlot, tail) == 4,
                  pe1_global_slot_tail_offset);
PE1_STATIC_ASSERT(sizeof(Pe1GlobalSlot) == 0x0C, pe1_global_slot_size);

#endif
