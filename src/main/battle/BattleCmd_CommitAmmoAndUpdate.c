#include "common.h"
#include "pe1/battle.h"
#include "pe1/inventory.h"
extern void **D_8009D254;
extern short D_800C0E08;
extern s8 D_800C0E20;

int Inv_IsActiveListOverrideSelected(void);
void Inv_SelectActiveList(int mode);
int Inv_DrawSlotItemIcon(void);
void Inv_BuildWeaponList(int unused, void *out);

#define COMBATANT_FIELD(base, type, member) \
    (*(type)((char *)(base) + PE1_OFFSETOF(Combatant, member)))
#define ACTION_FIELD(base, type, member) \
    (*(type)((char *)(base) + PE1_OFFSETOF(BattleAction, member)))
#define ITEM_FIELD(base, type, member) \
    (*(type)((char *)(base) + PE1_OFFSETOF(ItemDataRecord, member)))

int BattleCmd_CommitAmmoAndUpdate(void *out) {
    void *current;
    void *entry;
    int saved;
    int result;

    if (D_8009D254 != 0) {
        current = D_8009D254[0];
        if (current != 0) {
            D_800C0E08 = COMBATANT_FIELD(current, unsigned short *, curHP);
            if (COMBATANT_FIELD(current, void **, action) == 0) {
                goto tail;
            }
            saved = Inv_IsActiveListOverrideSelected();
            Inv_SelectActiveList(0);
            entry = Inv_LookupActiveListData(D_800C0E20);
            if (entry != 0) {
                ITEM_FIELD(entry, short *, ammo) = ACTION_FIELD(
                    COMBATANT_FIELD(current, void **, action), int *, attackWord) & 0x3FF;
            }
            Inv_SelectActiveList(saved);
        }
    }

tail:
    result = Inv_DrawSlotItemIcon();
    Inv_BuildWeaponList(0, out);
    return result;
}

#undef ACTION_FIELD
#undef COMBATANT_FIELD
#undef ITEM_FIELD
