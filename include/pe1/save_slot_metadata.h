#ifndef PE1_SAVE_SLOT_METADATA_H
#define PE1_SAVE_SLOT_METADATA_H

#include "common.h"

/* Summary of the save slot shown by Save_DrawSlotMetadata. */
typedef struct SaveSlotSummary {
    u8 reserved00[0x10];
    int primaryValue;   /* 0x10: nothing is drawn while it is not positive */
    u8 reserved14[0x74];
    int secondaryValue; /* 0x88 */
} SaveSlotSummary;

/*
 * Save prompt state word: low byte counts frames down, bits 8-9 hold the
 * phase, bits 10-11 the prompt kind and bits 12-18 one pending message each.
 * Retail addresses the word and its low byte as separate objects, so the
 * frame counter has its own symbol at the same address (little-endian).
 */
extern u8 g_SavePromptTimer;

extern SaveSlotSummary **D_8009D1A8;
extern unsigned int D_8009D1AC;

/* Prompt texts: two fixed lines, then per-window tables. */
extern u8 D_80091464[];
extern u8 D_80091474[];
extern u8 D_80091480[][22];
extern u8 D_800914AC[][20];
extern u8 D_800914D4[][20];
extern u8 D_800914FC[][18];
extern u8 D_80091520[][18];
extern u8 D_80091544[][22];
extern u8 D_80091570[][21];

#endif
