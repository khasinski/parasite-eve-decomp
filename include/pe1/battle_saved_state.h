#ifndef PE1_BATTLE_SAVED_STATE_H
#define PE1_BATTLE_SAVED_STATE_H

#include "common.h"
#include "pe1/battle.h"

/* Word-aligned battle views over opaque save records serialized as bytes. */
typedef struct BattleStateTail {
    s32 words[6];
} BattleStateTail;
PE1_STATIC_ASSERT(sizeof(BattleStateTail) == 0x18, battle_state_tail_size);

/* Saved 0x70-byte prefix of Aya's Combatant record; the last two words are
 * Combatant.action and Combatant.attributes. */
typedef struct AyaBattleState {
    s32 words[26];
/* 0x68 */ BattleStateTail *action;
/* 0x6C */ BattleAttributes *attributes;
} AyaBattleState;
PE1_STATIC_ASSERT(sizeof(AyaBattleState) == 0x70, aya_battle_state_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(AyaBattleState, action) == 0x68,
                  aya_battle_state_action_offset);

/* Rodata defaults Battle_SaveAyaState loads into the saved battle records. */
extern AyaBattleState D_80010928;
extern BattleStateTail D_80010998;

extern AyaBattleState g_AyaBattleState __asm__("D_800B8A20");
extern BattleStateTail g_SavedBattleStateTail __asm__("D_800B0CB0");

#endif /* PE1_BATTLE_SAVED_STATE_H */
