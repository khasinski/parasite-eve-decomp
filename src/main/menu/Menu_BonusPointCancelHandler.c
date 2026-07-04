/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK MenuWidget_ClearCursorY();
s32 MenuWidget_FindByModeAndSelectedBase();
void *MenuWidget_GetCurrentNode();
M2C_UNK MenuWidget_SetCurrentNode();
M2C_UNK Menu_StepInventoryRoot();
M2C_UNK Menu_OpenBonusPointSpendDialog();
M2C_UNK Menu_PlayConfirmSound();
M2C_UNK Menu_PlayCancelSound();
s32 MenuWidget_GetChild();
M2C_UNK MenuWidget_NavScrollTo();
s32 MenuWidget_GridCellIndex();
extern s32 g_MenuEquipMode;
extern s32 g_MenuItemRenameMode;
extern s32 g_BonusPointDisplayValue;
extern u16 g_BonusPointStatDeltas[];
#define g_BonusPointStatDeltas (g_BonusPointStatDeltas[0])
extern u16 g_AyaStatAgility[];
#define g_AyaStatAgility (g_AyaStatAgility[0])
extern s32 g_AyaBonusPoints[];
#define g_AyaBonusPoints (g_AyaBonusPoints[0])

s32 Menu_BonusPointCancelHandler(void *arg0, s32 arg1) {
    s32 temp_s0;
    s32 var_a0;
    u16 *var_a1;
    u16 *var_v1;
    u16 temp_v0;

    temp_s0 = MenuWidget_GetChild(arg0, 0);
    if (arg1 & 0x10000) {
        Menu_OpenBonusPointSpendDialog(temp_s0, MenuWidget_GridCellIndex(temp_s0));
        Menu_PlayConfirmSound();
        return 1;
    }
    if (arg1 & 0x40) {
        MenuWidget_NavScrollTo(0xF);
        MenuWidget_NavScrollTo(0xB);
        MenuWidget_NavScrollTo(0xD);
        MenuWidget_NavScrollTo(0x18);
        MenuWidget_NavScrollTo(0x30);
        if (g_MenuEquipMode != 0) {
            MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0x32));
        } else {
            if (MenuWidget_GetCurrentNode() != NULL) {
                MenuWidget_ClearCursorY(M2C_FIELD(MenuWidget_GetCurrentNode(), s32 *, 4));
            }
            MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
        }
        g_MenuItemRenameMode = 0;
        var_a1 = &g_AyaStatAgility;
        var_a0 = 0;
        var_v1 = &g_BonusPointStatDeltas;
        do {
            temp_v0 = *var_v1;
            var_v1 += 2;
            var_a0 += 1;
            *var_a1 = temp_v0;
            var_a1 += 1;
        } while (var_a0 < 7);
        g_AyaBonusPoints = g_BonusPointDisplayValue;
        Menu_StepInventoryRoot(0x37E, -1, -1);
        Menu_PlayCancelSound();
    }
    return 1;
}
