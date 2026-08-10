#ifndef PE1_MEMCARD_H
#define PE1_MEMCARD_H

#include "common.h"

typedef struct MemCardSaveSlot {
    u8 state;
    u8 present;
    u8 reserved02[2];
    u8 primaryTitle[8];
    s32 playTime;
    u8 reserved10[4];
    u8 alternateTitle[12];
    s32 blendColor;
    s16 currentHp;
    s16 maxHp;
    u8 levelIndex;
    u8 titleStyleFlag;
    u8 bonusIndex;
    u8 reserved2B;
    s16 locationNumber;
    s16 locationNameId;
    u8 reserved30[0x14];
} MemCardSaveSlot;

typedef struct MemCardPortState {
    u8 present;
    u8 managerState;
    u8 fileCount;
    u8 reserved03[5];
    u8 cardState;
    u8 reserved09[0x13];
    MemCardSaveSlot slots[15];
} MemCardPortState;

PE1_STATIC_ASSERT(sizeof(MemCardSaveSlot) == 0x44, memcard_save_slot_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MemCardSaveSlot, playTime) == 0x0C,
                  memcard_save_slot_play_time_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MemCardSaveSlot, blendColor) == 0x20,
                  memcard_save_slot_blend_color_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MemCardSaveSlot, levelIndex) == 0x28,
                  memcard_save_slot_level_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MemCardSaveSlot, locationNumber) == 0x2C,
                  memcard_save_slot_location_offset);
PE1_STATIC_ASSERT(sizeof(MemCardPortState) == 0x418, memcard_port_state_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MemCardPortState, slots) == 0x1C,
                  memcard_port_state_slots_offset);

extern MemCardPortState g_MemCardPortStates[];
extern int g_MemCardActivePortOneBased;
extern int g_MemCardActiveState;
extern int g_MemCardActivePromptPending;
extern int g_MemCardLoadSucceeded;

#endif
