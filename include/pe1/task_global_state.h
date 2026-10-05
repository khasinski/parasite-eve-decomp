#ifndef PE1_TASK_GLOBAL_STATE_H
#define PE1_TASK_GLOBAL_STATE_H

#include "common.h"

/* Several task handlers expose one global word followed by the same
 * eight-byte continuation area, while interpreting the word with different
 * signedness. */
typedef union Pe1TaskGlobalWord {
    s32 signed_value;
    u32 unsigned_value;
} Pe1TaskGlobalWord;

typedef struct Pe1TaskGlobalWordSlot {
    /* 0x00 */ Pe1TaskGlobalWord value;
    /* 0x04 */ u8 continuation[8];
} Pe1TaskGlobalWordSlot;

PE1_STATIC_ASSERT(sizeof(Pe1TaskGlobalWord) == 4, pe1_task_global_word_size);
PE1_STATIC_ASSERT(sizeof(Pe1TaskGlobalWordSlot) == 0x0C,
                  pe1_task_global_word_slot_size);

#endif
