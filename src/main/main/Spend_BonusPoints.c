#include "pe1/inventory.h"

/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int D_8009CFAC;
extern int D_8009CFD0;
extern int D_8009CFD8;
extern int D_8009CFDC;
extern int D_8009CF68;
extern int D_800A18D8[];
extern short D_800A1A0E[], D_800A1A10[], D_800A1A12[];
extern signed char D_800C0E20[];
extern signed char D_800C0E22[];

extern int Inv_RestoreSelection(unsigned int index);
extern ItemDataRecord *Inv_LookupActiveListData(int index);
extern void Stat_QueryDistanceAndSubLevel(int kind, int value, int *distance_out, int *sublevel_out);
extern int Stat_OverflowQuery(int kind, int value);
extern void Menu_PlayMoveSound(void);
extern void Menu_PlayErrorSound(void);
extern void Menu_PlayConfirmSound(void);
extern void Menu_PlayCancelSound(void);
extern void MenuWidget_DestroyNode(void *node);
extern void Inv_SetActiveList(int list, int selected);

int Spend_BonusPoints(void *node, unsigned int buttons)
{
    ItemDataRecord *item;
    int amount;
    register int refund asm("$2");
    int changed;

    item = Inv_LookupActiveListData(Inv_RestoreSelection(0));
    if (buttons & 0x4000) {
        D_8009CFAC = 0;
        if (D_8009CFD8 < 100) {
            goto error;
        }
        changed = 0;
        switch (D_8009CFD0) {
        case 0: {
            short *bonus = &D_800A1A0E[0];
            changed = *bonus < 999;
            *bonus += changed;
            break;
        }
        case 1: {
            short *bonus = &D_800A1A10[0];
            changed = *bonus < 999;
            *bonus += changed;
            break;
        }
        case 2: {
            short *bonus = &D_800A1A12[0];
            changed = *bonus < 999;
            *bonus += changed;
            break;
        }
        case 5:
        case 6:
            Stat_QueryDistanceAndSubLevel(D_8009CFD0, D_8009CFDC, &amount, 0);
            changed = amount > 0;
            D_8009CFDC += amount;
            break;
        }
        D_8009CFD8 -= changed * 100;
        if (!changed) {
            goto error;
        }
        Menu_PlayMoveSound();
        return 1;
    }

    if (buttons & 0x1000) {
        D_8009CFAC = 1;
        switch (D_8009CFD0) {
        case 0: {
            short *bonus = &D_800A1A0E[0];
            if (*bonus <= item->bonusStats[0]) {
                goto error;
            }
            {
                short next;
                refund = D_8009CFD8;
                asm volatile("" : : "r"(refund));
                next = *bonus - 1;
                asm volatile("" ::: "memory");
                *bonus = next;
            }
            break;
        }
        case 1: {
            short *bonus = &D_800A1A10[0];
            if (*bonus <= item->bonusStats[1]) {
                goto error;
            }
            {
                short next;
                refund = D_8009CFD8;
                asm volatile("" : : "r"(refund));
                next = *bonus - 1;
                asm volatile("" ::: "memory");
                *bonus = next;
            }
            break;
        }
        case 2: {
            short *bonus = &D_800A1A12[0];
            if (*bonus <= item->bonusStats[2]) {
                goto error;
            }
            --*bonus;
            refund = D_8009CFD8;
            break;
        }
        case 5:
        case 6: {
            int *minimum = &D_800A18D8[D_8009CFD0];
            int gap;
            int new_current;
            if (*minimum >= D_8009CFDC) {
                goto error;
            }
            gap = Stat_OverflowQuery(D_8009CFD0, D_8009CFDC);
            if (D_8009CFDC - gap < *minimum) {
                new_current = D_800A18D8[D_8009CFD0];
            } else {
                new_current = D_8009CFDC - Stat_OverflowQuery(D_8009CFD0, D_8009CFDC);
            }
            D_8009CFDC = new_current;
            refund = D_8009CFD8;
            break;
        }
        default:
            return 1;
        }
        D_8009CFD8 = refund + 100;
        Menu_PlayMoveSound();
        return 1;
    }

    goto after_error;
error:
    Menu_PlayErrorSound();
    return 1;
after_error:
    if (buttons & 0x10000) {
        D_8009CF68 = D_8009CFD8;
        if (D_8009CFD0 < 3) {
            *item = D_800A1A00;
            if (!Inv_IsActiveListOverrideSelected()) {
                if (item == Inv_LookupActiveListData(D_800C0E20[0])) {
                    Inv_SetActiveList(2, 0);
                } else if (item == Inv_LookupActiveListData(D_800C0E22[0])) {
                    Inv_SetActiveList(3, 0);
                }
            }
        } else {
            D_800A18D8[D_8009CFD0] = D_8009CFDC;
        }
        MenuWidget_DestroyNode(node);
        Menu_PlayConfirmSound();
        return 1;
    }

    if (buttons & 0x40) {
        MenuWidget_DestroyNode(node);
        Menu_PlayCancelSound();
    }
    return 1;
}
