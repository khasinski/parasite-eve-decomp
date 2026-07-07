/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
void *MenuWidget_FindByModeAndSelectedBase(M2C_UNK, M2C_UNK); /* extern */
s32 MenuWidget_FindLastMode1WithCursorX();          /* extern */
M2C_UNK MenuWidget_SetCurrentNode(void *);          /* extern */
M2C_UNK MenuWidget_DestroyNode(void *);                      /* extern */
M2C_UNK MenuWidget_NavScrollTo(s32);                         /* extern */
extern s32 g_SaveSelectedSlot;
extern M2C_UNK (*g_MenuDeferredCallback)();

void Menu_CloseSaveSlotListView(s32 arg0) {
    void *temp_s0;

    temp_s0 = MenuWidget_FindByModeAndSelectedBase(2, 0x24);
    MenuWidget_NavScrollTo(0x28);
    if (g_MenuDeferredCallback != NULL) {
        g_MenuDeferredCallback();
        g_MenuDeferredCallback = NULL;
    }
    if (arg0 == g_SaveSelectedSlot) {
        MenuWidget_NavScrollTo(0x3F);
        MenuWidget_NavScrollTo(0x27);
        MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x29));
        MenuWidget_NavScrollTo(0x1F);
        MenuWidget_NavScrollTo(arg0 + 0x25);
        if (temp_s0 != NULL) {
            M2C_FIELD(temp_s0, s32 *, 0x44) = 0;
            if (MenuWidget_FindLastMode1WithCursorX() == 0) {
                MenuWidget_SetCurrentNode(temp_s0);
            }
        }
    }
}
