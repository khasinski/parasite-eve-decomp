/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 Inv_GetPackedListCount();
void *MenuWidget_CreateNode();
void *MenuWidget_FindByModeAndSelectedBase();
s32 MenuWidget_IsCursorYClear();
M2C_UNK MenuWidget_NavScrollTo();
M2C_UNK MenuWidget_SetCurrentNode();
M2C_UNK Menu_CreateBonusPointAllocationView();
M2C_UNK func_80047678();
M2C_UNK Menu_OpenItemList();
s32 Battle_IsInputAllowedWrapped();
M2C_UNK Menu_PlayConfirmSound();
M2C_UNK Menu_PlayCancelSound();
M2C_UNK Menu_PlayMoveSound();
M2C_UNK Menu_PlayErrorSound();
void *MenuWidget_GetChild();
s32 MenuWidget_CreateSimpleNode();
M2C_UNK MenuWidget_DestroyNode();
extern s32 g_MenuLayoutLocked;
extern M2C_UNK Menu_SetupSkillSubmenu[];
#define Menu_SetupSkillSubmenu (Menu_SetupSkillSubmenu[0])

s32 Menu_StepSkillScreen(void *arg0, s32 arg1) {
    void *parg0;
    s32 parg1;
    s32 tv4k;
    s32 temp_v0;
    register void *temp_s0 asm("$16");
    register void *temp_v0_2 asm("$16");
    register void *temp_v0_3 asm("$18");
    register void *temp_v0_4 asm("$16");
    register void *temp_v0_5 asm("$16");
    register s32 cm1 asm("$17");

    parg0 = arg0;
    parg1 = arg1;
    temp_s0 = MenuWidget_GetChild(arg0, NULL);
    if (parg1 & 0x10000) {
        if (Inv_GetPackedListCount() != 0) {
            if (g_MenuLayoutLocked != 0) {
                Menu_OpenItemList();
                return 1;
            }
            if (Battle_IsInputAllowedWrapped() != 0) {
                temp_v0 = MenuWidget_CreateSimpleNode(0x35, 0, 0, 0);
                M2C_FIELD(MenuWidget_CreateNode(0x35, temp_v0, temp_v0), M2C_UNK **, 0x30) = &Menu_SetupSkillSubmenu;
                temp_v0_2 = MenuWidget_FindByModeAndSelectedBase(2, 7);
                __asm__("" : "=r"(temp_v0_2) : "0"(temp_v0_2));
                M2C_FIELD(temp_v0_2, s32 *, 0x44) = 0;
                MenuWidget_SetCurrentNode(temp_v0_2);
                Menu_PlayConfirmSound();
                return 1;
            }
            goto block_6;
        }
block_6:
        Menu_PlayErrorSound();
        return 1;
    }
    if (parg1 & 0x40) {
        if (g_MenuLayoutLocked != 0) {
            func_80047678();
            return 1;
        }
        MenuWidget_NavScrollTo(7);
        MenuWidget_DestroyNode(parg0);
        temp_v0_3 = MenuWidget_FindByModeAndSelectedBase(1, 6);
        __asm__("" : "=r"(temp_v0_3) : "0"(temp_v0_3));
        if (temp_v0_3 != NULL) {
            MenuWidget_DestroyNode(temp_v0_3);
            Menu_CreateBonusPointAllocationView();
            Menu_PlayCancelSound();
            return 1;
        }
        /* Duplicate return node #20. Try simplifying control flow for better match */
        return 1;
    }
    tv4k = parg1 & 0x4000;
    if (tv4k && (MenuWidget_IsCursorYClear(MenuWidget_FindByModeAndSelectedBase(1, 6)) != 0)) {
        cm1 = -1;
        __asm__("" : "=r"(cm1) : "0"(cm1));
        M2C_FIELD(temp_s0, s32 *, 0x44) = cm1;
        temp_v0_4 = MenuWidget_GetChild(parg0, 1);
        __asm__("" : "=r"(temp_v0_4) : "0"(temp_v0_4));
        if (temp_v0_4 != NULL) {
            M2C_FIELD(temp_v0_4, s32 *, 0x44) = cm1;
        }
        temp_v0_5 = MenuWidget_FindByModeAndSelectedBase(2, 6);
        __asm__("" : "=r"(temp_v0_5) : "0"(temp_v0_5));
        if (temp_v0_5 != NULL) {
            M2C_FIELD(temp_v0_5, s32 *, 0x48) = 0;
            M2C_FIELD(temp_v0_5, s32 *, 0x44) = 0;
            MenuWidget_SetCurrentNode(temp_v0_5);
        }
        Menu_PlayMoveSound();
    }
    return 1;
}
