/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "include_asm.h"
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK MenuWidget_OffsetPosition();
M2C_UNK MenuWidget_SetCurrentNode();
M2C_UNK Menu_OpenInventoryOrSwapView();
M2C_UNK Menu_CreateEquipInfoPanel();
M2C_UNK Menu_PlayConfirmSound();
M2C_UNK Menu_PlayCancelSound();
M2C_UNK Sort_ContainerItems();
s32 MenuWidget_GetChild();
void *MenuWidget_CreateSimpleNode();
void *MenuWidget_CreateNode();
s32 MenuWidget_GridCellIndex();
extern s32 g_MenuInventoryViewMode;
extern M2C_UNK Menu_DrawEquipSortToggleList[];
#define Menu_DrawEquipSortToggleList (Menu_DrawEquipSortToggleList[0])
extern M2C_UNK Menu_EquipGridHandler[];
#define Menu_EquipGridHandler (Menu_EquipGridHandler[0])

asm(".globl func_800471E4");
asm("func_800471E4 = Menu_EquipOptionsInputHandler");

s32 Menu_EquipOptionsInputHandler(s32 arg0, s32 arg1) {
    s32 temp_a1;
    s32 temp_v0;
    void *temp_v0_2;
    void *temp_v0_3;

    if (arg1 & 0x10000) {
        temp_v0 = MenuWidget_GetChild(arg0, 0);
        temp_a1 = MenuWidget_GridCellIndex(temp_v0);
        switch (g_MenuInventoryViewMode) {                       /* irregular */
        case 0:
            if (temp_a1 == 2) {
                temp_v0_2 = MenuWidget_CreateSimpleNode(0x3B, temp_v0, 0, 0);
                temp_v0_3 = MenuWidget_CreateNode(0x3B, temp_v0_2, temp_v0_2);
                M2C_FIELD(temp_v0_2, M2C_UNK **, 0x2C) = &Menu_EquipGridHandler;
                MenuWidget_OffsetPosition(temp_v0_2, 0x70 - M2C_FIELD(temp_v0_2, s32 *, 0x18), 0x48 - M2C_FIELD(temp_v0_2, s32 *, 0x1C));
                M2C_FIELD(temp_v0_3, M2C_UNK **, 0x30) = &Menu_DrawEquipSortToggleList;
                MenuWidget_SetCurrentNode(temp_v0_3);
            } else {
            case 2:
                Menu_CreateEquipInfoPanel(temp_v0, temp_a1);
            }
            break;
        case 1:
            Sort_ContainerItems(temp_a1, temp_a1);
            Menu_OpenInventoryOrSwapView(1);
            break;
        }
        Menu_PlayConfirmSound();
        return 1;
    }
    if (arg1 & 0x40) {
        Menu_OpenInventoryOrSwapView(0);
        Menu_PlayCancelSound();
    }
    return 1;
}
