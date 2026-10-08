#ifndef PE1_FIELD_ENGINE_SLOT_H
#define PE1_FIELD_ENGINE_SLOT_H

#include "common.h"

/* One dispatch slot in g_FieldEngineSlotTable (0x40 entries, 6 bytes each).
 * The field engine walks the table every frame: flag==1 dispatches the
 * handler_id'th handler, flag==2 marks the slot for teardown. */
typedef struct FieldEngSlot {
    /* 0x0 */ unsigned char handler_id;
    /* 0x1 */ signed char flag;
    /* 0x2 */ unsigned short counter;
    /* 0x4 */ short data_offset;
} FieldEngSlot;

PE1_STATIC_ASSERT(sizeof(FieldEngSlot) == 6, field_engine_slot_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldEngSlot, flag) == 1, field_engine_slot_flag);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldEngSlot, counter) == 2, field_engine_slot_counter);
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldEngSlot, data_offset) == 4,
                  field_engine_slot_data_offset);

#endif
