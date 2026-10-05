#include "common.h"
#include "pe1/psyq_nop.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/inventory.h"
#include "pe1/menu_inventory.h"

extern s32 g_BonusPointDisplayValue;
extern s32 g_MenuSpendArrowDirection;
extern s32 g_BonusPointSpendStat;
extern s32 g_BonusPointSpendWorkingValue;
extern s32 g_BonusPointSpendCurrentValue;
extern s32 g_BonusPointStatDeltas[];

void *MenuWidget_CreateSimpleNode(s32 arg0, void *arg1, void *arg2, s32 arg3);
void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
void MenuWidget_OffsetPosition(MenuWidgetNode *ptr, int dx, int dy);
s32 Inv_RestoreSelection(u32 index);
void Menu_DrawBonusPointSpendPanel(void);
s32 Spend_BonusPoints(void *node, u32 buttons);

void Menu_OpenBonusPointSpendDialog(MenuWidgetNode *arg0, s32 arg1) {
    MenuWidgetNode *node;
    InvItemSlot *record;

    node = MenuWidget_CreateSimpleNode(9, arg0, 0, 1);
    node->draw = Menu_DrawBonusPointSpendPanel;
    node->update = Spend_BonusPoints;
    node->field_28 = 1;
    MenuWidget_SetCurrentNode(node);

    g_BonusPointSpendStat = arg1;
    g_BonusPointSpendWorkingValue = g_BonusPointDisplayValue;
    if (arg1 < 3) {
        record = Inv_LookupActiveListData(Inv_RestoreSelection(0));
        D_800A1A00 = *record;
        node->grid_width -= 0x14;
        MenuWidget_OffsetPosition(node, 0xA, 0);
    } else {
        s32 value = g_BonusPointStatDeltas[arg1];
        PE1_NOP();
        g_BonusPointSpendCurrentValue = value;
    }
    g_MenuSpendArrowDirection = 0;
}
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/inventory.h"

extern int g_MenuSpendArrowDirection;
extern int g_BonusPointSpendStat;
extern int g_BonusPointSpendWorkingValue;
extern int g_BonusPointSpendCurrentValue;
extern int D_8009CF18;
void Draw_OffsetCursor(int, int);
void Draw_AllocSprite(int);
void Draw_PrintNumberWidth5(int);
void Draw_PrintNumberWidth4(int);
void Draw_PrintNumberWidth4Unk(int);
void Draw_PrintSignedNumberWidth4(int);
void Draw_PrintNumberWidth2(int);
void Draw_AllocTexturedRectAlt(int, int);
void Stat_QueryLevelAndSubLevel(int, int, int *, int *);
void Stat_QueryDistanceAndSubLevel(int, int, int *, int *);

void Menu_DrawBonusPointSpendPanel(void) {
    u8 *stat;
    int value;
    int mode;
    int level;
    int distance;
    int sublevel;
    register int sprite_index asm("$2");

    Draw_OffsetCursor(0x14, 0x15);
    Draw_AllocSprite(g_MenuSpendArrowDirection + 0x4D);
    Draw_OffsetCursor(-0xE, -0x11);
    Draw_AllocSprite(0x93);

    if (g_BonusPointSpendStat < 3) {
        Draw_OffsetCursor(0x48, 0);
        Draw_PrintNumberWidth5(g_BonusPointSpendWorkingValue);
        Draw_OffsetCursor(-0x75, 0x1E);
        if (D_8009CF18 != 0) {
            sprite_index = g_BonusPointSpendStat;
            Draw_AllocSprite(sprite_index + 0x7C);
        } else {
            sprite_index = g_BonusPointSpendStat;
            Draw_AllocSprite(sprite_index + 0x7F);
        }
        Draw_OffsetCursor(0x1E, 0);
        mode = g_BonusPointSpendStat;
        switch (mode) {
        case 0: {
            stat = &D_800A1A00.baseStats[0];
            /* Keep the bonus load relative to the selected stat pointer. */
            asm volatile("" : "=r"(stat) : "0"(stat));
            value = *stat + *(s16 *)(stat + 7);
            if (value >= 1000) value = 999;
            Draw_PrintNumberWidth4(value);
            Draw_OffsetCursor(4, 2);
            Draw_PrintNumberWidth4Unk(*stat);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(*(s16 *)(stat + 7));
            break;
        }
        case 1: {
            stat = &D_800A1A00.baseStats[1];
            asm volatile("" : "=r"(stat) : "0"(stat));
            value = *stat + *(s16 *)(stat + 8);
            if (value >= 1000) value = 999;
            Draw_PrintNumberWidth4(value);
            Draw_OffsetCursor(4, 2);
            Draw_PrintNumberWidth4Unk(*stat);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(*(s16 *)(stat + 8));
            break;
        }
        case 2: {
            stat = &D_800A1A00.baseStats[2];
            asm volatile("" : "=r"(stat) : "0"(stat));
            value = *stat + *(s16 *)(stat + 9);
            if (value >= 1000) value = 999;
            Draw_PrintNumberWidth4(value);
            Draw_OffsetCursor(4, 2);
            Draw_PrintNumberWidth4Unk(*stat);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(*(s16 *)(stat + 9));
            break;
        }
        }
        Draw_OffsetCursor(-0x2D, -0xA);
        Draw_AllocSprite(0x87);
        Draw_OffsetCursor(0x19, 0);
        Draw_AllocSprite(0x88);
    } else {
        Draw_OffsetCursor(0x5C, 0);
        Draw_PrintNumberWidth5(g_BonusPointSpendWorkingValue);
        Draw_OffsetCursor(-0x8A, 0x1F);
        Draw_AllocSprite(g_BonusPointSpendStat + 0x8C);
        Draw_OffsetCursor(0x42, -1);
        Stat_QueryLevelAndSubLevel(g_BonusPointSpendStat,
                                   g_BonusPointSpendCurrentValue, &level, 0);
        Draw_PrintNumberWidth2(level + 1);
        Stat_QueryDistanceAndSubLevel(g_BonusPointSpendStat,
                                      g_BonusPointSpendCurrentValue, &distance, &sublevel);
        Draw_OffsetCursor(2, 0);
        Draw_AllocTexturedRectAlt(distance, sublevel);
    }
}
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
