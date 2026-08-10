#include "pe1/battle.h"
#include "pe1/inventory.h"

extern void **g_PlayerEntity;
extern short g_AyaHpCurrent;
extern signed char g_AyaEquippedWeaponSlot;

void Battle_ApplySpellEffect(int, void **);
int Inv_IsActiveListOverrideSelected(void);
void Inv_SelectActiveList(int);
extern unsigned int g_AyaParasiteSpellFlags;

#define COMBATANT_FIELD(base, type, member) \
    (*(type)((char *)(base) + PE1_OFFSETOF(Combatant, member)))
#define ACTION_FIELD(base, type, member) \
    (*(type)((char *)(base) + PE1_OFFSETOF(BattleAction, member)))
#define ITEM_FIELD(base, type, member) \
    (*(type)((char *)(base) + PE1_OFFSETOF(ItemDataRecord, member)))

void BattleCmd_ChangeWeaponAndSync(int arg0) {
    void *current;
    void *entry;
    int saved;

    Battle_ApplySpellEffect(arg0, g_PlayerEntity);
    if (g_PlayerEntity != 0) {
        current = g_PlayerEntity[0];
        if (current != 0) {
            g_AyaHpCurrent = COMBATANT_FIELD(current, unsigned short *, curHP);
            if (COMBATANT_FIELD(current, void **, action) == 0) {
                goto out;
            }
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
out:;
}

#undef ITEM_FIELD
#undef ACTION_FIELD
#undef COMBATANT_FIELD

int Aya_HasParasiteSpell(int spell)
{
    return (g_AyaParasiteSpellFlags >> spell) & 1;
}
