#include "pe1/battle.h"
#include "pe1/inventory.h"

extern void **g_PlayerEntity;
extern short g_AyaHpCurrent;
extern signed char g_AyaEquippedWeaponSlot;

int Inv_IsActiveListOverrideSelected(void);
void Inv_SelectActiveList(int);
extern unsigned int g_AyaParasiteSpellFlags;


void BattleCmd_ChangeWeaponAndSync(int arg0) {
    Combatant *current;
    ItemDataRecord *entry;
    int saved;

    Battle_ApplySpellEffect(arg0, (BattleEntity *)g_PlayerEntity);
    if (g_PlayerEntity != 0) {
        current = g_PlayerEntity[0];
        if (current != 0) {
            g_AyaHpCurrent = current->curHP;
            if (*(void **)&current->action != 0) {
                saved = Inv_IsActiveListOverrideSelected();
                Inv_SelectActiveList(0);
                entry = Inv_LookupActiveListData(g_AyaEquippedWeaponSlot);
                if (entry != 0) {
                    *(short *)&entry->ammo = *(int *)&((BattleAction *)*(void **)&current->action)->attackWord & 0x3FF;
                }
                Inv_SelectActiveList(saved);
            }
        }
    }
}


int Aya_HasParasiteSpell(int spell)
{
    return (g_AyaParasiteSpellFlags >> spell) & 1;
}
