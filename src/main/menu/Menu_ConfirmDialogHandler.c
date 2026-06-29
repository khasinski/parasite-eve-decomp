/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */

typedef int s32;

s32 MenuWidget_GridCellIndex(s32 arg0);
void MenuWidget_RestoreSavedCurrentNode(void);
void Menu_PlayConfirmSound(void) __asm__("func_800525EC");
void Menu_PlayCancelSound(void) __asm__("func_80052634");
s32 MenuWidget_GetChild(void *arg0, s32 arg1) __asm__("func_80062A20");
void MenuWidget_DestroyNode(void *arg0) __asm__("func_80062F1C");

extern void (*g_MenuConfirmCallback)(void *arg0, s32 arg1);

s32 Menu_ConfirmDialogHandler(void *arg0, s32 arg1) {
    s32 index;

    if (arg1 & 0x10000) {
        index = MenuWidget_GridCellIndex(MenuWidget_GetChild(arg0, 0));
        switch (index) {
        case 0:
            MenuWidget_DestroyNode(arg0);
            if (*(s32 *)((char *)arg0 + 0x24) == 0x2A) {
                MenuWidget_RestoreSavedCurrentNode();
            }
            if (g_MenuConfirmCallback != 0) {
                g_MenuConfirmCallback(arg0, 1);
            }
            Menu_PlayConfirmSound();
            break;
        case 1:
            MenuWidget_DestroyNode(arg0);
            if (*(s32 *)((char *)arg0 + 0x24) == 0x2A) {
                MenuWidget_RestoreSavedCurrentNode();
            }
            if (g_MenuConfirmCallback != 0) {
                g_MenuConfirmCallback(arg0, 0);
            }
            Menu_PlayCancelSound();
            break;
        }
        return 1;
    }

    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        if (*(s32 *)((char *)arg0 + 0x24) == 0x2A) {
            MenuWidget_RestoreSavedCurrentNode();
        }
        if (g_MenuConfirmCallback != 0) {
            g_MenuConfirmCallback(arg0, 0);
        }
        Menu_PlayCancelSound();
    }
    return 1;
}
