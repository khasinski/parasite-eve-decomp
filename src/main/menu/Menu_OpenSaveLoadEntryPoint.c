#include "common.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_inventory.h"
#include "include_asm.h"

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

void Queue_Init(void);
s32 Inv_GetPackedListCursor(void);
void Menu_StepInventoryRoot(s32 arg0, s32 arg1, s32 arg2);
void Menu_RequestErrorSound(void);
void Menu_OpenRenameScreen(s32 arg0);

void Menu_DrawContextHelpText(void);

void Menu_OpenSaveLoadEntryPoint(s32 arg0) {
    Queue_Init();
    if ((MenuWidget_FindByModeAndSelectedBase(1, 0xD) == NULL) && (MenuWidget_FindByModeAndSelectedBase(1, 0x17) == NULL)) {
        if (arg0 != 0) {
            if (Inv_GetPackedListCursor() != 0) {
                Menu_StepInventoryRoot(0, -2, -1);
            } else {
                Menu_RequestErrorSound();
                return;
            }
        } else {
            Menu_OpenRenameScreen(-1);
        }
        if (MenuWidget_FindByModeAndSelectedBase(1, 0x13) == NULL) {
            M2C_FIELD(MenuWidget_CreateSimpleNode(0x13, 0, 0, 0), void **, 0x30) = Menu_DrawContextHelpText;
        }
    }
}
