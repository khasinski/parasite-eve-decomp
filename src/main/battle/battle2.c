#include "include_asm.h"

/* Incomplete array: retail materializes the flag word's address
 * (0x800C0E24) with lui/addiu before the read-modify-write. */
extern unsigned int g_AyaParasiteSpellFlags[];

extern void **g_PlayerEntity;
extern short g_AyaHpCurrent;
extern signed char g_AyaEquippedWeaponSlot;

int Inv_IsActiveListOverrideSelected(void);
void Inv_SelectActiveList(int);
void *Inv_LookupActiveListData(int);
int Inv_DrawSlotItemIcon(void);
void Inv_BuildWeaponList(int, int);

extern int g_StatScaleBase;
unsigned short *Aya_LookupLevelStats(int arg0);

void Aya_UnlockParasiteSpell(int spell) {
    g_AyaParasiteSpellFlags[0] |= 1u << spell;
}

int Math_IntSqrt(int value)
{
    int result = 0;
    int shift = 30;

    do {
        int step = ((result << 2) + 1) << shift;

        result <<= 1;
        if (value >= step) {
            value -= step;
            result |= 1;
        }

        shift -= 2;
    } while (shift >= 0);

    return result;
}

int BattleCmd_CommitAmmoAndUpdate(int arg0) {
    void *current;
    void *entry;
    int saved;
    int result;

    if (g_PlayerEntity != 0) {
        current = g_PlayerEntity[0];
        if (current != 0) {
            g_AyaHpCurrent = *(unsigned short *)((char *)current + 0xC);
            if (*(void **)((char *)current + 0x68) == 0) {
                goto tail;
            }
            saved = Inv_IsActiveListOverrideSelected();
            Inv_SelectActiveList(0);
            entry = Inv_LookupActiveListData(g_AyaEquippedWeaponSlot);
            if (entry != 0) {
                *(short *)((char *)entry + 0xA) =
                    *(int *)((char *)*(void **)((char *)current + 0x68) + 0xC) & 0x3FF;
            }
            Inv_SelectActiveList(saved);
        }
    }

tail:
    result = Inv_DrawSlotItemIcon();
    Inv_BuildWeaponList(0, arg0);
    return result;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/battle/battle2", Inv_BuildWeaponList);

INCLUDE_ASM("asm/USA/main/nonmatchings/battle/battle2", BattleCmd_LoadWeaponModifiers);

int Battle_GetScaledMaxHP(int arg0) {
    unsigned short value = *Aya_LookupLevelStats(arg0);
    return ((g_StatScaleBase + 0x14) * value) / 20;
}
