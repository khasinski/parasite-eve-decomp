#ifndef PE1_BATTLE_SAVED_STATE_H
#define PE1_BATTLE_SAVED_STATE_H

#include "common.h"

/* Word-aligned battle views over opaque save records serialized as bytes. */
typedef struct AyaBattleState {
    s32 words[28];
} AyaBattleState;
PE1_STATIC_ASSERT(sizeof(AyaBattleState) == 0x70, aya_battle_state_size);

typedef struct BattleStateTail {
    s32 words[6];
} BattleStateTail;
PE1_STATIC_ASSERT(sizeof(BattleStateTail) == 0x18, battle_state_tail_size);

extern AyaBattleState g_AyaBattleState __asm__("D_800B8A20");
extern BattleStateTail g_SavedBattleStateTail __asm__("D_800B0CB0");

#endif /* PE1_BATTLE_SAVED_STATE_H */
