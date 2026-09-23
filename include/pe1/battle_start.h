#ifndef PE1_BATTLE_START_H
#define PE1_BATTLE_START_H

#include "pe1/battle.h"

extern Combatant *D_8009D278;
extern BattleEntity *D_8009D254[4];
extern BattleRewardSlot D_800A7FF0[10];
extern u32 D_800B8A90[7];
extern int D_8009D1E8;
extern u32 D_8009D28C;
extern u8 D_8009CE7C, D_8009CE78, D_8009D288, D_8009CE74;
extern u32 D_8009D1A8[4];
extern u8 D_8009D1CE[16], D_8009D235[16];
extern u32 D_8009D304;
extern u16 D_8009D21C;

/* Separate views preserve the original absolute-address loads and stores. */
extern u8 battle_start_flags_byte[16] __asm__("D_8009D1AC");
extern u32 battle_start_flags_read[4] __asm__("D_8009D1AC");
extern u32 battle_start_flags_write[4] __asm__("D_8009D1AC");
extern BattleEntity *battle_start_final_entity[4] __asm__("D_8009D254");
extern struct { char bytes[16]; } battle_start_phase_storage __asm__("D_8009D290");
extern struct { char bytes[16]; } battle_start_seed_storage __asm__("D_8009D250");
extern struct { char bytes[16]; } battle_start_camera_storage __asm__("D_8009D27C");

void Save_ResetGlobalFlags(void);
void srand(int seed);
void Battle_ResetEnemyStats(int mode);
void Battle_SetupEnemyAnims(void);
void Battle_CheckDropChance(void);
int Entity_CheckActionIdMatch(void);
void Window_SetBoundsByMode(int mode);
void Entity_SetActionMode(BattleEntity *entity, int mode);
void Battle_StartEncounter(int mode);

#endif
