/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */

#include "include_asm.h"
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

void MenuWidget_SetCurrentNode(void *node);
void *MenuWidget_CreateSimpleNode(int mode, int arg1, int arg2, int arg3);
void *MenuWidget_CreateNode(int mode, void *parent, void *sibling);
void MenuWidget_SetColumnLayout(void *node, int value);
void MenuWidget_ClearColumnLayout(void *node);

extern s32 g_InvItemUsableFlag;
extern s32 g_MenuLayoutLocked;
extern struct { char _[16]; } func_8004620C_o __asm__("Menu_InventoryPageInputHandler");
#define Menu_InventoryPageInputHandler (*(void *)&func_8004620C_o)
extern struct { char _[16]; } func_8004F9A0_o __asm__("Menu_DrawUsableItemActionList");
#define Menu_DrawUsableItemActionList (*(void *)&func_8004F9A0_o)

asm(".globl func_80045EE4");
asm("func_80045EE4 = Menu_CreateInventoryListView");

void Menu_CreateInventoryListView(s32 parent) {
    void *root;
    void *node;
    s32 flags;
    s32 is_layout_locked;

    root = MenuWidget_CreateSimpleNode(6, parent, 0, 0);
    node = MenuWidget_CreateNode(6, root, root);
    M2C_FIELD(root, void **, 0x2C) = &Menu_InventoryPageInputHandler;
    M2C_FIELD(root, s32 *, 0x40) = 1;
    M2C_FIELD(node, void **, 0x30) = &Menu_DrawUsableItemActionList;
    asm("" : : : "memory");
    flags = M2C_FIELD(node, volatile s32 *, 0x64);
    is_layout_locked = g_MenuLayoutLocked;
    flags |= 0x80;
    M2C_FIELD(node, s32 *, 0x64) = flags;
    if (is_layout_locked != 0) {
        MenuWidget_ClearColumnLayout(node);
    } else if (g_InvItemUsableFlag == 0) {
        MenuWidget_SetColumnLayout(node, 0x14);
    }
    if (M2C_FIELD(node, s32 *, 0x44) >= 0) {
        MenuWidget_SetCurrentNode(node);
    }
}
