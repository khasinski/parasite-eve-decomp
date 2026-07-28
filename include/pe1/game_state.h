#ifndef PE1_GAME_STATE_H
#define PE1_GAME_STATE_H

typedef signed char Pe1S8;
typedef unsigned char Pe1U8;
typedef unsigned int Pe1U32;

/*
 * Shared scene, field, and AKAO state at 0x800B0CD8 (g_GameState).
 * Only fields corroborated by multiple main-executable users are named;
 * padding keeps the observed retail offsets stable while the remaining
 * substructures are reconstructed.
 */
typedef struct Pe1GameState {
    Pe1U32 flags;                    /* 0x000 */
    Pe1U8 unk_004[0x08];
    Pe1S8 current_story_day;         /* 0x00C */
    Pe1S8 pending_story_day;         /* 0x00D */
    Pe1U8 story_day_flags;           /* 0x00E */
    Pe1U8 unk_00f[0x05];
    Pe1U8 scene_work[0x0C4];         /* 0x014 */
    Pe1U8 draw_prim_a[0x10];         /* 0x0D8 */
    Pe1U8 unk_0e8[0x01C];
    Pe1U8 draw_prim_b[0x10];         /* 0x104 */
    Pe1U8 draw_prim_c[0x10];         /* 0x114 */
    Pe1U8 unk_124[0x02C];
    Pe1U32 voice_bank_base;          /* 0x150 */
    Pe1U32 voice_bank_base_1400;     /* 0x154 */
    Pe1U32 voice_bank_base_2800;     /* 0x158 */
    Pe1U8 unk_15c[0x34];
} Pe1GameState;

extern Pe1GameState g_GameState;

#endif
