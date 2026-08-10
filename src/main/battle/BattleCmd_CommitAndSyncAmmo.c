
/* Incomplete arrays: retail addresses these absolutely (g_PlayerEntity is
 * in the gp window but not small data; the Aya fields are outside it). */
#include "pe1/battle.h"

extern void **g_PlayerEntity[];
extern short g_AyaHpCurrent[];
extern signed char g_AyaEquippedWeaponSlot[];

void Battle_ApplyDamage(void);
int Inv_IsActiveListOverrideSelected(void);
void Inv_SelectActiveList(int useOverride);
void *Inv_LookupActiveListData(int index);

#define COMBATANT_FIELD(base, type, member) \
    (*(type)((char *)(base) + PE1_OFFSETOF(Combatant, member)))
#define ACTION_FIELD(base, type, member) \
    (*(type)((char *)(base) + PE1_OFFSETOF(BattleAction, member)))

void BattleCmd_CommitAndSyncAmmo(void) {
    void **entity;
    void *current;
    void *entry;
    int saved;

    Battle_ApplyDamage();
    entity = g_PlayerEntity[0];
    if (entity != 0) {
        current = entity[0];
        if (current != 0) {
            g_AyaHpCurrent[0] = COMBATANT_FIELD(current, unsigned short *, curHP);
            if (COMBATANT_FIELD(current, void **, action) != 0) {
                saved = Inv_IsActiveListOverrideSelected();
                Inv_SelectActiveList(0);
                entry = Inv_LookupActiveListData(g_AyaEquippedWeaponSlot[0]);
                if (entry != 0) {
                    *(short *)((char *)entry + 0xA) = ACTION_FIELD(
                        COMBATANT_FIELD(current, void **, action), int *, attackWord) & 0x3FF;
                }
                Inv_SelectActiveList(saved);
            }
        }
    }
}

#undef ACTION_FIELD
#undef COMBATANT_FIELD
