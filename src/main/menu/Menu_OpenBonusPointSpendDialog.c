/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

#include "pe1/inventory.h"

typedef struct MenuWidgetNode {
    char pad00[0x28];
    s32 field28;
    void *field2C;
    void *field30;
    s32 field34;
} MenuWidgetNode;

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
void *Inv_LookupActiveListData(int index);
void Menu_DrawBonusPointSpendPanel(void);
s32 Spend_BonusPoints(s32);

void Menu_OpenBonusPointSpendDialog(s32 arg0, s32 arg1) {
    MenuWidgetNode *node;
    InvItemSlot *record;

    node = MenuWidget_CreateSimpleNode(9, arg0, 0, 1);
    node->field30 = Menu_DrawBonusPointSpendPanel;
    node->field2C = Spend_BonusPoints;
    node->field28 = 1;
    MenuWidget_SetCurrentNode(node);

    g_BonusPointSpendStat = arg1;
    g_BonusPointSpendWorkingValue = g_BonusPointDisplayValue;
    if (arg1 < 3) {
        record = Inv_LookupActiveListData(Inv_RestoreSelection(0));
        D_800A1A00 = *record;
        node->field34 -= 0x14;
        MenuWidget_OffsetPosition(node, 0xA, 0);
    } else {
        s32 value = g_BonusPointStatDeltas[arg1];
        __asm__ volatile("nop");
        g_BonusPointSpendCurrentValue = value;
    }
    g_MenuSpendArrowDirection = 0;
}
