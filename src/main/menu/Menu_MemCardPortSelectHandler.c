#include "common.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_inventory.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

s32 MemCard_IsPortPresent(s32 port);
void MemCard_SetDialogActive(s32 active);
int MenuWidget_GetCellIndex(MenuWidgetNode *ptr);
s32 MemCard_CheckPresent(s32 port);
void MemCard_StartRead(s32 port, s32 slot);
void MemCard_CloseAllAndResetState(void);
s32 MemCard_IsOperationPending(void);
void Menu_PlayConfirmSound(void);
void Menu_PlayCancelSound(void);
void Menu_PlayErrorSound(void);
extern s32 g_MenuMemCardConfirmPending;

s32 Menu_MemCardPortSelectHandler(void *node, s32 flags) {
    s32 cellIndex;

    cellIndex = MenuWidget_GetCellIndex(MenuWidget_GetChild(node, 0));
    if (flags & 0x10000) {
        if (cellIndex >= 0) {
            if (cellIndex < 2) {
                goto read_card;
            }
            if (cellIndex == 2) {
                goto close_dialog;
            }
            goto error_sound;
read_card:
            if ((MemCard_CheckPresent(cellIndex) != 0) && (MemCard_IsPortPresent(cellIndex) != 0) && (MemCard_IsOperationPending() == 0)) {
                MemCard_CloseAllAndResetState();
                MemCard_StartRead(cellIndex, 1);
                Menu_PlayConfirmSound();
                return 1;
            }
            return 1;
close_dialog:
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x25));
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x26));
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x24));
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x13));
            MemCard_SetDialogActive(0);
            Inv_SetActiveList(9, 0);
            g_MenuMemCardConfirmPending = 0;
            Menu_PlayConfirmSound();
            return 1;
        }
error_sound:
        Menu_PlayErrorSound();
        return 1;
    }
    if (flags & 0x40) {
        MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x25));
        MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x26));
        MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x24));
        MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x13));
        MemCard_SetDialogActive(0);
        Inv_SetActiveList(9, 0);
        g_MenuMemCardConfirmPending = 0;
        Menu_PlayCancelSound();
    }
    return 1;
}
