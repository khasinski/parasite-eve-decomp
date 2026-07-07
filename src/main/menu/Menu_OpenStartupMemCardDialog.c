/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "include_asm.h"
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

void MemCard_InitState(void);
void MemCard_SetDialogActive(s32 active);
void *MenuWidget_FindByModeAndSelectedBase(s32 mode, s32 base);
void *MenuWidget_GetCurrentNode(void);
void MenuWidget_SetCurrentNode(void *node);
void Queue_Init(void);
void *MenuWidget_CreateSimpleNode(s32 mode, s32 arg1, s32 arg2, s32 arg3);
void *MenuWidget_CreateNode(s32 mode, void *parent, void *sibling);

extern s32 g_McDialogMode;
void Menu_DrawContextHelpText(void);
void Menu_MemCardPortSelectHandler(void);
void Menu_IsMemCardSlotSelectable(void);
void Menu_DrawMemCardPortList(void);

asm(".globl func_8004D18C");
asm("func_8004D18C = Menu_OpenStartupMemCardDialog");

void Menu_OpenStartupMemCardDialog(void) {
    s32 one;
    void *callback_2c;
    void *callback_30;
    void *callback_8c;
    void *child;
    void *parent;

    parent = MenuWidget_CreateSimpleNode(0x24, (s32)MenuWidget_GetCurrentNode(), 0, 0);
    child = MenuWidget_CreateNode(0x24, parent, parent);
    callback_2c = Menu_MemCardPortSelectHandler;
    callback_30 = Menu_DrawMemCardPortList;
    M2C_FIELD(parent, void **, 0x2C) = callback_2c;
    M2C_FIELD(child, void **, 0x30) = callback_30;
    callback_8c = Menu_IsMemCardSlotSelectable;
    M2C_FIELD(child, void **, 0x8C) = callback_8c;
    one = 1;
    if (M2C_FIELD(child, s32 *, 0x48) == 2) {
        M2C_FIELD(child, s32 *, 0x48) = 0;
    }
    MenuWidget_SetCurrentNode(child);
    if (MenuWidget_FindByModeAndSelectedBase(1, 0x13) == NULL) {
        M2C_FIELD(MenuWidget_CreateSimpleNode(0x13, 0, 0, 0), void **, 0x30) = Menu_DrawContextHelpText;
    }
    M2C_FIELD(MenuWidget_FindByModeAndSelectedBase(1, 0x13), s32 *, 0x38) = 0x24;
    g_McDialogMode = one;
    MemCard_SetDialogActive(1);
    MemCard_InitState();
    Queue_Init();
}
