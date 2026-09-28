#include "pe1/menu_widget.h"

/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int D_8009CEF0, D_8009CF18, D_8009CF1C, D_8009CF0C;
extern int D_8009CF30, D_8009CEF8;
extern int D_800A1888[], D_800A188C[], D_800A1890[], D_800A1894[];

extern MenuWidgetNode *MenuWidget_GetChild(MenuWidgetNode *node, int index);
extern int MenuWidget_GridCellIndex(MenuWidgetNode *node);
extern int Menu_GetBattleEquipMode(void);
extern void MenuWidget_NavScrollTo(int mode);
extern void Menu_CreateEquipScreen(MenuWidgetNode *node);
extern void Menu_CreateSkillActionScreen(MenuWidgetNode *node);
extern void Menu_CreateInventoryListView(MenuWidgetNode *node);
extern void Menu_CreateEquipItemSelectionView(MenuWidgetNode *node);
extern MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int base);
extern int Menu_StepSkillList(MenuWidgetNode *node, int buttons);
extern void Menu_CreateInventoryListView3(MenuWidgetNode *) asm("Menu_CreateInventoryListView");
extern void Menu_CreateEquipItemSelectionView3(MenuWidgetNode *) asm("Menu_CreateEquipItemSelectionView");
extern MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase3(int, int) asm("MenuWidget_FindByModeAndSelectedBase");
extern int Menu_StepSkillList3(MenuWidgetNode *, int) asm("Menu_StepSkillList");
extern int Battle_IsActiveWrapped(void);
extern void Inv_SetActiveList(int list, int selected);
extern void Menu_CreateMainMenuView(MenuWidgetNode *node);
extern void Menu_CreateInvSwapView(MenuWidgetNode *node, int mode);
extern int Inv_SelectActiveList(int list);
extern int Inv_CountByCategory(int category);
extern int WayneStorage_CountItemType(int item);
extern void Menu_StepInventoryRoot(int mode, int arg1, int arg2);
extern void Menu_PlayConfirmSound(void);
extern void Menu_PlayErrorSound(void);
extern void Menu_PlayCancelSound(void);
extern void MenuWidget_InitPool(void);
extern void BattleCmd_ResetTableCursor(void);
extern void Menu_InitScrollDirection(void);

#define CLOSE_SELECT_VIEWS() do { \
    MenuWidget_NavScrollTo(0x2d); \
    MenuWidget_NavScrollTo(0x18); \
    MenuWidget_NavScrollTo(0x12); \
} while (0)

/* Matching debt: the three inventory-root arguments retain retail registers.
 * Empty barriers preserve the return paths and call delay slots. */
int Menu_StepItemSelectScreen(MenuWidgetNode *node, unsigned int buttons)
{
    int index = 0;
    int mask;
    int action;
    MenuWidgetNode *child;
    int confirm = buttons & 0x10000;
    if (confirm) {
        child = MenuWidget_GetChild(node, 0);
        index = MenuWidget_GridCellIndex(child);
        if (Menu_GetBattleEquipMode()) {
            mask = D_8009CEF0 & 0x1f;
        } else {
            mask = D_8009CEF0 & 0x1ef;
        }
        action = -1;
        goto check;
        while (1) {
            if (index < 0) break;
            index -= mask & 1;
            mask >>= 1;
            ++action;
        check:
            if (action >= 9) break;
        }
        switch (action) {
        case 0:
            CLOSE_SELECT_VIEWS();
            Menu_CreateEquipScreen(child);
            break;
        case 1:
            CLOSE_SELECT_VIEWS();
            Menu_CreateSkillActionScreen(child);
            break;
        case 2:
            CLOSE_SELECT_VIEWS();
            D_8009CF18 = 1;
            Menu_CreateInventoryListView(child);
            Menu_CreateEquipItemSelectionView(child);
            Menu_StepSkillList(MenuWidget_FindByModeAndSelectedBase(2, 5), 0);
            break;
        case 3:
            CLOSE_SELECT_VIEWS();
            D_8009CF18 = 0;
            Menu_CreateInventoryListView3(child);
            Menu_CreateEquipItemSelectionView3(child);
            Menu_StepSkillList3(MenuWidget_FindByModeAndSelectedBase3(2, 5), 0);
            break;
        case 4:
            if (!Battle_IsActiveWrapped()) goto battle_error;
            CLOSE_SELECT_VIEWS();
            Inv_SetActiveList(8, 0);
            break;
        battle_error:
            Menu_PlayErrorSound();
            index = 1;
            asm volatile("" : "=r"(index) : "0"(index));
            return index;
        case 5:
            CLOSE_SELECT_VIEWS();
            Menu_CreateMainMenuView(child);
            break;
        case 6:
            CLOSE_SELECT_VIEWS();
            Menu_CreateInvSwapView(child, 0);
            break;
        case 7:
            CLOSE_SELECT_VIEWS();
            D_8009CF1C = 1;
            Inv_SelectActiveList(0);
            D_800A1888[0] = Inv_CountByCategory(0xe) ? 999 : Inv_CountByCategory(0xc);
            D_800A188C[0] = Inv_CountByCategory(0xf) ? 999 : Inv_CountByCategory(0xd);
            if (D_8009CF0C) {
                D_800A1890[0] = WayneStorage_CountItemType(0xe) ? 999 : WayneStorage_CountItemType(0xc);
                D_800A1894[0] = WayneStorage_CountItemType(0xf) ? 999 : WayneStorage_CountItemType(0xd);
            } else {
                D_800A1894[0] = 0;
                D_800A1890[0] = 0;
            }
            Menu_StepInventoryRoot(0x33e, -1, -1);
            D_8009CEF8 = 0;
            break;
        case 8: {
            int command;
            register int first asm("$4");
            register int second asm("$5");
            register int third asm("$6");
            CLOSE_SELECT_VIEWS();
            first = 0x37e;
            second = -1;
            third = -1;
            command = 1;
            asm volatile("" : "=r"(command) : "0"(command), "r"(first), "r"(second), "r"(third));
            D_8009CF30 = command;
            Menu_StepInventoryRoot(first, second, third);
            D_8009CEF8 = 0;
            break;
        }
        default:
            goto return_one;
        }
        Menu_PlayConfirmSound();
        index = 1;
        asm volatile("" : "=r"(index) : "0"(index));
        return index;
    }
    if (buttons & 0x40) {
        MenuWidget_InitPool();
        Inv_SetActiveList(9, 0);
        if (!Menu_GetBattleEquipMode()) BattleCmd_ResetTableCursor();
        Menu_PlayCancelSound();
    return_one:
        index = 1;
        asm volatile("" : "=r"(index) : "0"(index));
        return index;
    }
    if (({
            int down = buttons & 2;
            asm volatile("" : "=r"(index) : "0"(index));
            down;
        })) {
        Menu_InitScrollDirection();
    }
    return index;
}
