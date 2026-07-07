/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;

#define NULL ((void *)0)

#include "../../../tools/m2c/m2c_macros.h"

void MenuWidget_ClearCursorY();
void *MenuWidget_FindByModeAndSelectedBase();
void Menu_OpenEquipScreen();
void Menu_PlayConfirmSound(void);
void Menu_PlayCancelSound(void);
void Inv_RemoveActiveListItem();
void *MenuWidget_GetChild();
void MenuWidget_DestroyNode(s32 node);
void MenuWidget_NavScrollTo(s32 selected_base);
s32 MenuWidget_GridCellIndex();

extern s32 g_MenuEquipSwapSource;
extern s32 D_8009CF9C;
extern s16 g_AyaInventoryItems[];

s32 Menu_HandleAyaInventorySelectionInput(s32 arg0, s32 arg1);

s32 Menu_HandleAyaInventorySelectionInput(s32 arg0, s32 arg1)
{
    s32 temp_v0_2;
    void *temp_v0;

    if (arg1 & 0x10000) {
        temp_v0 = MenuWidget_FindByModeAndSelectedBase(2, 1);
        {
            s32 item_id;

            item_id = g_AyaInventoryItems[MenuWidget_GridCellIndex(temp_v0)];
            __asm__ volatile("nop");
            D_8009CF9C = item_id;
        }
        temp_v0_2 = MenuWidget_GridCellIndex(temp_v0) | (M2C_FIELD(temp_v0, s32 *, 0x5C) << 8);
        g_MenuEquipSwapSource = temp_v0_2 + 1;
        Inv_RemoveActiveListItem(temp_v0_2 & 0xFF);
        MenuWidget_DestroyNode(arg0);
        MenuWidget_NavScrollTo(0x1B);
        MenuWidget_NavScrollTo(1);
        MenuWidget_NavScrollTo(0);
        Menu_OpenEquipScreen(MenuWidget_GridCellIndex(MenuWidget_GetChild(arg0, 0)));
        Menu_PlayConfirmSound();
        return 1;
    }
    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0));
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 1));
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
        Menu_PlayCancelSound();
    }
    return 1;
}
