
/* Incomplete arrays: retail addresses these absolutely (g_PlayerEntity is
 * in the gp window but not small data; the Aya fields are outside it). */
#include "pe1/battle.h"
#include "pe1/inventory.h"

extern void **g_PlayerEntity[];
extern short g_AyaHpCurrent[];
extern signed char g_AyaEquippedWeaponSlot[];

int Inv_IsActiveListOverrideSelected(void);
void Inv_SelectActiveList(int useOverride);
#define COMBATANT_FIELD(base, type, member) \
    (*(type)((char *)(base) + PE1_OFFSETOF(Combatant, member)))
#define ACTION_FIELD(base, type, member) \
    (*(type)((char *)(base) + PE1_OFFSETOF(BattleAction, member)))
#define ITEM_FIELD(base, type, member) \
    (*(type)((char *)(base) + PE1_OFFSETOF(ItemDataRecord, member)))

void BattleCmd_SyncActiveAmmo(void) {
    void **entity;
    void *current;
    void *entry;
    int saved;

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
                    ITEM_FIELD(entry, short *, ammo) = ACTION_FIELD(
                        COMBATANT_FIELD(current, void **, action), int *, attackWord) & 0x3FF;
                }
                Inv_SelectActiveList(saved);
            }
        }
    }
}

#undef ACTION_FIELD
#undef COMBATANT_FIELD
#undef ITEM_FIELD
