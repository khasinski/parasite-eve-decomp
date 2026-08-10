#ifndef PE1_MEMCARD_H
#define PE1_MEMCARD_H

#include "common.h"

enum MemCardSaveSlotState {
    MEMCARD_SLOT_OCCUPIED = 1,
    MEMCARD_SLOT_EMPTY = 2,
    MEMCARD_SLOT_UNAVAILABLE = 3
};

/* Compact menu-side copy made from the first 0x80 bytes of the saved runtime
 * state.  The card scanner keeps one preview per port at D_800A1720 and copies
 * the selected fields below into MemCardSaveSlot. */
typedef struct MemCardSavePreview {
    u8 primaryTitle[8];
    s32 playTimeMinutes;
    s32 gameTimeMinutes;
    u8 alternateTitle[12];
    u8 reserved1C[0x0A];
    u16 maxHp;
    u16 currentHp;
    u8 levelIndex;
    u8 exGameIndex;
    u8 reserved2C[0x30];
    s16 progressStage;
    s16 mapNumber;
    u8 reserved60[4];
    s32 blendColor;
    u8 reserved68[0x18];
} MemCardSavePreview;

typedef struct MemCardSaveSlot {
    u8 state;
    u8 metadataReady;
    u8 reserved02[2];
    u8 primaryTitle[8];
    s32 playTimeMinutes;
    s32 gameTimeMinutes;
    u8 alternateTitle[12];
    s32 blendColor;
    s16 currentHp;
    s16 maxHp;
    u8 levelIndex;
    u8 titleStyleFlag;
    u8 exGameIndex;
    u8 reserved2B;
    s16 progressStage;
    s16 mapNumber;
    u8 reserved30[0x14];
} MemCardSaveSlot;

typedef struct MemCardPortState {
    u8 present;
    u8 managerState;
    u8 fileCount;
    u8 selectedSlot;
    u8 hasFiles;
    u8 listCursor;
    u8 scanCursor;
    u8 pendingError;
    u8 cardState;
    u8 reserved09;
    u8 sequence;
    u8 nextState;
    s32 fileDescriptor;
    u8 reserved10[4];
    s16 transferSize;
    s16 retryCount;
    u8 *transferData;
    MemCardSaveSlot slots[15];
} MemCardPortState;

PE1_STATIC_ASSERT(sizeof(MemCardSaveSlot) == 0x44, memcard_save_slot_size);
PE1_STATIC_ASSERT(sizeof(MemCardSavePreview) == 0x80,
                  memcard_save_preview_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MemCardSavePreview, maxHp) == 0x26,
                  memcard_save_preview_max_hp_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MemCardSavePreview, progressStage) == 0x5C,
                  memcard_save_preview_progress_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MemCardSavePreview, blendColor) == 0x64,
                  memcard_save_preview_blend_color_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MemCardSaveSlot, playTimeMinutes) == 0x0C,
                  memcard_save_slot_play_time_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MemCardSaveSlot, blendColor) == 0x20,
                  memcard_save_slot_blend_color_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MemCardSaveSlot, levelIndex) == 0x28,
                  memcard_save_slot_level_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MemCardSaveSlot, progressStage) == 0x2C,
                  memcard_save_slot_progress_offset);
PE1_STATIC_ASSERT(sizeof(MemCardPortState) == 0x418, memcard_port_state_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MemCardPortState, fileDescriptor) == 0x0C,
                  memcard_port_state_file_descriptor_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MemCardPortState, transferData) == 0x18,
                  memcard_port_state_transfer_data_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MemCardPortState, slots) == 0x1C,
                  memcard_port_state_slots_offset);

extern MemCardPortState g_MemCardPortStates[];
extern MemCardSavePreview D_800A1720[];
extern int g_MemCardActivePortOneBased;
extern MemCardPortState *g_MemCardActiveState;
extern int g_MemCardActiveBytesRemaining;
extern int g_MemCardActivePromptPending;
extern int g_MemCardLoadSucceeded;

#endif
