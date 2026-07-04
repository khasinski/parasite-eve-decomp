/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

void Queue_Init(void);
void *MenuWidget_FindByModeAndSelectedBase(s32 mode, s32 base);
s32 Inv_GetPackedListCursor(void);
void Menu_StepInventoryRoot(s32 arg0, s32 arg1, s32 arg2);
void Menu_RequestErrorSound(void);
void Menu_OpenRenameScreen(s32 arg0);
void *MenuWidget_CreateSimpleNode(s32 mode, s32 arg1, s32 arg2, s32 arg3);

void Menu_DrawContextHelpText(void);

asm(".globl func_8004DCA4");
asm("func_8004DCA4 = Menu_OpenSaveLoadEntryPoint");

void Menu_OpenSaveLoadEntryPoint(s32 arg0) {
    Queue_Init();
    if ((MenuWidget_FindByModeAndSelectedBase(1, 0xD) == NULL) && (MenuWidget_FindByModeAndSelectedBase(1, 0x17) == NULL)) {
        if (arg0 != 0) {
            if (Inv_GetPackedListCursor() != 0) {
                Menu_StepInventoryRoot(0, -2, -1);
                goto ensure_context_help;
            }
            Menu_RequestErrorSound();
            return;
        }
        Menu_OpenRenameScreen(-1);
ensure_context_help:
        if (MenuWidget_FindByModeAndSelectedBase(1, 0x13) == NULL) {
            M2C_FIELD(MenuWidget_CreateSimpleNode(0x13, 0, 0, 0), void **, 0x30) = Menu_DrawContextHelpText;
        }
    }
}
