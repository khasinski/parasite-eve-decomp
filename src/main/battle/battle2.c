/* MASPSX_FLAGS: --store-call-delay */
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

typedef signed char s8;
typedef unsigned char u8;

typedef struct {
    unsigned char pad[0x14];
    u8 modifier_count;
    u8 modifiers[1];
} WeaponEntry;

extern int D_8009D018;
extern int D_800A1B30[];
extern int D_800A1B48[];
extern struct { char _[16]; } D_800C0E22_obj __asm__("D_800C0E22");
#define D_800C0E22 (*(s8 *)&D_800C0E22_obj)

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

void BattleCmd_LoadWeaponModifiers(void) {
    WeaponEntry *entry;
    int saved;
    int i;
    int modifier;
    int modifier_count;
    int *stat;
    int pad[2];

    asm volatile("" : "=m"(pad));
    saved = Inv_IsActiveListOverrideSelected();
    Inv_SelectActiveList(0);

    i = 6;
    stat = D_800A1B48;
    asm volatile("sw $zero,%%gp_rel(D_8009D018)($gp)" ::: "memory");
    do {
        *stat = 0;
        i--;
        stat--;
    } while (i >= 0);

    entry = Inv_LookupActiveListData(D_800C0E22);
    if (entry != 0) {
        modifier_count = entry->modifier_count;
        asm volatile("" : "=r"(modifier_count) : "0"(modifier_count));
        if (modifier_count <= 0) {
            goto done_modifiers;
        }
        i = 0;
        do {
            modifier = entry->modifiers[i] & 0x1F;
            switch (modifier) {
            case 8:
            case 9:
            case 10:
                asm volatile("sw %0,%%gp_rel(D_8009D018)($gp)" : : "r"(1 << (modifier - 8)) : "memory");
                break;
            case 11:
                D_800A1B30[0] = 3;
                break;
            case 12:
                D_800A1B30[1] = 2;
                break;
            case 13:
                D_800A1B30[5] = -2;
                break;
            case 15:
                D_800A1B30[1] = -2;
                break;
            }
            i++;
        } while (i < entry->modifier_count);
    }

done_modifiers:
    Inv_RecalcSlotStats();
    Inv_SelectActiveList(0);
    Inv_SelectActiveList(saved);
}

int Battle_GetScaledMaxHP(int arg0) {
    unsigned short value = *Aya_LookupLevelStats(arg0);
    return ((g_StatScaleBase + 0x14) * value) / 20;
}
