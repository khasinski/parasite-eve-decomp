/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

void MenuWidget_SetCurrentNode();
void Menu_InitStateTables(void);
void *MenuWidget_CreateSimpleNode(s32 mode, s32 arg1, s32 arg2, s32 arg3);
void *MenuWidget_CreateNode(s32 mode, void *parent, void *sibling);

extern s32 g_MenuBonusPointBarAnimActive;
extern struct { char _[16]; } D_80092314_o __asm__("D_80092314");
#define D_80092314 (*(void *)&D_80092314_o)
extern struct { char _[16]; } func_8004BF40_o __asm__("Menu_DrawBonusPointAnimFrame");
#define Menu_DrawBonusPointAnimFrame (*(void *)&func_8004BF40_o)
extern struct { char _[16]; } func_8004C1E0_o __asm__("Menu_StepParasiteScreen");
#define Menu_StepParasiteScreen (*(void *)&func_8004C1E0_o)
extern struct { char _[16]; } func_8004FFF8_o __asm__("Menu_DrawStatAllocationList");
#define Menu_DrawStatAllocationList (*(void *)&func_8004FFF8_o)

void Menu_CreateLevelUpResultPanel(void) {
    s32 current_y;
    register s32 pos asm("$3");
    register void *callback asm("$3");
    void *root;
    void *child;
    register void *current asm("$4");

    root = MenuWidget_CreateSimpleNode(0x15, 0, 0, 0);
    child = MenuWidget_CreateNode(0x2F, root, root);
    current = root;
    callback = &Menu_DrawBonusPointAnimFrame;
    M2C_FIELD(root, void **, 0x30) = callback;
    callback = &Menu_StepParasiteScreen;
    M2C_FIELD(root, void **, 0x2C) = callback;
    callback = &Menu_DrawStatAllocationList;
    M2C_FIELD(child, void **, 0x30) = callback;
    pos = M2C_FIELD(child, s32 *, 0x18);
    current_y = -1;
    M2C_FIELD(child, s32 *, 0x44) = current_y;
    current_y = M2C_FIELD(child, s32 *, 0x40);
    pos += 0x44;
    M2C_FIELD(child, s32 *, 0x18) = pos;
    pos = M2C_FIELD(child, s32 *, 0x1C);
    current_y -= 2;
    M2C_FIELD(child, s32 *, 0x40) = current_y;
    pos += 2;
    M2C_FIELD(child, s32 *, 0x1C) = pos;
    MenuWidget_SetCurrentNode(current, current_y);
    M2C_FIELD(root, void **, 0x4C) = &D_80092314;
    g_MenuBonusPointBarAnimActive = 1;
    Menu_InitStateTables();
}
