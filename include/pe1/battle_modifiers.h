#ifndef PE1_BATTLE_MODIFIERS_H
#define PE1_BATTLE_MODIFIERS_H

#include "pe1/inventory.h"

int Inv_IsActiveListOverrideSelected(void);
void Inv_SelectActiveList(int useOverride);
void Inv_RecalcSlotStats(void);

/* Seven modifier words cleared before scanning the equipped item record. */
extern int D_800A1B30[7];
extern int D_8009D018;
/* Existing byte-array view of the equipped armor index in Aya's save state. */
extern signed char D_800C0E22[];

/* Historical name: retail selects the equipped armor slot at 0x800C0E22. */
void BattleCmd_LoadWeaponModifiers(void);

#endif
