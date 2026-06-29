/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 MenuWidget_FindByModeAndSelectedBase();
M2C_UNK MenuWidget_SetCurrentNode();
s32 Save_GetMetadataWindowIndex();
M2C_UNK Save_LoadMetadataWindowText();
M2C_UNK Save_SelectMetadataWindow();
M2C_UNK Inv_ClearEquipFlagForKind();
s32 func_8005332C();
void *func_80062D2C();
void *MenuWidget_CreateNode();
extern struct { char _[16]; } D_80092354_o __asm__("D_80092354");
#define D_80092354 (*(M2C_UNK *)&D_80092354_o)
extern struct { char _[16]; } D_80092380_o __asm__("D_80092380");
#define D_80092380 (*(M2C_UNK *)&D_80092380_o)
extern struct { char _[16]; } D_800923A0_o __asm__("D_800923A0");
#define D_800923A0 (*(M2C_UNK *)&D_800923A0_o)
extern s32 g_MenuRenameTargetRecord;
extern struct { char _[16]; } D_800B0CD8_o __asm__("g_GameState");
#define g_GameState (*(s32 *)&D_800B0CD8_o)
extern struct { char _[16]; } func_8004DF74_o __asm__("func_8004DF74");
#define func_8004DF74 (*(M2C_UNK *)&func_8004DF74_o)
extern struct { char _[16]; } func_8004E074_o __asm__("Menu_StepNameEntry");
#define Menu_StepNameEntry (*(M2C_UNK *)&func_8004E074_o)
extern struct { char _[16]; } func_8004E2E4_o __asm__("Menu_StepNameEntryAlt");
#define Menu_StepNameEntryAlt (*(M2C_UNK *)&func_8004E2E4_o)
extern struct { char _[16]; } func_800500A8_o __asm__("Menu_DrawItemIconList");
#define Menu_DrawItemIconList (*(M2C_UNK *)&func_800500A8_o)
extern struct { char _[16]; } func_8005010C_o __asm__("Menu_DrawMemCardSlot1List");
#define Menu_DrawMemCardSlot1List (*(M2C_UNK *)&func_8005010C_o)
extern struct { char _[16]; } func_80050178_o __asm__("Menu_DrawMemCardSlot2List");
#define Menu_DrawMemCardSlot2List (*(M2C_UNK *)&func_80050178_o)
extern struct { char _[16]; } func_800501C8_o __asm__("Menu_DrawMemCardSlot3List");
#define Menu_DrawMemCardSlot3List (*(M2C_UNK *)&func_800501C8_o)

void Menu_OpenRenameScreen(s32 arg0) {
    M2C_UNK *var_v1;
    s32 temp_v0_5;
    void *temp_v0;
    void *temp_v0_3;
    void *temp_v0_4;
    void *var_a3;

    temp_v0 = func_80062D2C(0x17, 0, 0, 0);
    M2C_FIELD(temp_v0, M2C_UNK **, 0x2C) = &Menu_StepNameEntryAlt;
    temp_v0_5 = Save_GetMetadataWindowIndex();
    var_v1 = &D_80092354;
    if (temp_v0_5 != 0) {
        var_v1 = &D_80092380;
    }
    M2C_FIELD(temp_v0, M2C_UNK **, 0x4C) = var_v1;
    if (Save_GetMetadataWindowIndex() != 0) {
        var_a3 = MenuWidget_CreateNode(0x18, temp_v0, temp_v0);
        M2C_FIELD(var_a3, M2C_UNK **, 0x30) = &Menu_DrawMemCardSlot2List;
        M2C_FIELD(var_a3, s32 *, 0x1C) = (s32) (M2C_FIELD(var_a3, s32 *, 0x1C) - 0x34);
        var_a3 = MenuWidget_CreateNode(0x19, temp_v0, temp_v0, var_a3);
        M2C_FIELD(var_a3, M2C_UNK **, 0x30) = &Menu_DrawMemCardSlot3List;
        M2C_FIELD(var_a3, s32 *, 0x1C) = (s32) (M2C_FIELD(var_a3, s32 *, 0x1C) - 0x34);
    } else {
        M2C_FIELD(MenuWidget_CreateNode(0x17, temp_v0, temp_v0), M2C_UNK **, 0x30) = &Menu_DrawMemCardSlot1List;
        M2C_FIELD(MenuWidget_CreateNode(0x18, temp_v0, temp_v0), M2C_UNK **, 0x30) = &Menu_DrawMemCardSlot2List;
        var_a3 = MenuWidget_CreateNode(0x19, temp_v0, temp_v0);
        M2C_FIELD(var_a3, M2C_UNK **, 0x30) = &Menu_DrawMemCardSlot3List;
    }
    M2C_FIELD(var_a3, s32 *, 0x44) = -1;
    temp_v0_3 = func_80062D2C(0x11, MenuWidget_FindByModeAndSelectedBase(2, 0x17), 0, 0);
    __asm__ volatile("");
    var_a3 = MenuWidget_CreateNode(0x11, temp_v0_3, temp_v0_3);
    M2C_FIELD(temp_v0_3, M2C_UNK **, 0x2C) = &Menu_StepNameEntry;
    M2C_FIELD(var_a3, M2C_UNK **, 0x30) = &Menu_DrawItemIconList;
    M2C_FIELD(var_a3, s32 *, 0x44) = 0;
    M2C_FIELD(var_a3, s32 *, 0x48) = 0;
    MenuWidget_SetCurrentNode(var_a3);
    temp_v0_4 = func_80062D2C(0x1A, 0, 0, 0);
    __asm__ volatile("");
    M2C_FIELD(temp_v0_4, M2C_UNK **, 0x30) = &func_8004DF74;
    temp_v0_5 = func_8005332C(arg0);
    g_MenuRenameTargetRecord = temp_v0_5;
    if (temp_v0_5 == 0) {
        M2C_FIELD(temp_v0_4, M2C_UNK **, 0x4C) = &D_800923A0;
    }
    Save_SelectMetadataWindow(g_MenuRenameTargetRecord);
    Save_LoadMetadataWindowText();
    Inv_ClearEquipFlagForKind(g_MenuRenameTargetRecord);
    g_GameState = g_GameState | 0x8000;
}
