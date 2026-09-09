#include "pe1/battle.h"
#include "pe1/inventory.h"

/* g_PlayerEntity points at the player-side field actor; its core is the
 * Combatant record used by the command paths below. */
extern BattleEntity *g_PlayerEntity;
extern s16 g_AyaHpCurrent;
extern s8 g_AyaEquippedWeaponSlot;

int Inv_IsActiveListOverrideSelected(void);
void Inv_SelectActiveList(int useOverride);
#define COMBATANT_FIELD(base, type, member) \
    (*(type)((char *)(base) + PE1_OFFSETOF(Combatant, member)))
#define ACTION_FIELD(base, type, member) \
    (*(type)((char *)(base) + PE1_OFFSETOF(BattleAction, member)))
#define ITEM_FIELD(base, type, member) \
    (*(type)((char *)(base) + PE1_OFFSETOF(ItemDataRecord, member)))

void BattleCmd_SyncActiveAmmo(void) {
    BattleEntity *entity;
    void *current;
    void *entry;
    int saved;

    entity = g_PlayerEntity;
    if (entity != 0) {
        current = entity->core;
        if (current != 0) {
            g_AyaHpCurrent = COMBATANT_FIELD(current, unsigned short *, curHP);
            if (COMBATANT_FIELD(current, void **, action) != 0) {
                saved = Inv_IsActiveListOverrideSelected();
                Inv_SelectActiveList(0);
                entry = Inv_LookupActiveListData(g_AyaEquippedWeaponSlot);
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

void BattleCmd_SetCurrentHP(int arg0) {
    Combatant *current;

    if (g_PlayerEntity != 0) {
        current = g_PlayerEntity->core;
        if (current != 0) {
            current->curHP = arg0;
        }
    }

    g_AyaHpCurrent = arg0;
}
