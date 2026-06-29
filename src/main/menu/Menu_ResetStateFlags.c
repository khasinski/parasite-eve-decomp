/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuSelectionLocked;
int g_MenuActionSubmenuOpen;
int g_MenuEquipMode;
int g_MenuLayoutLocked;
int g_MenuItemUseMode;
int g_MenuItemRenameMode;
int g_MenuItemDiscardMode;
int g_MenuItemSelectFlag;
int g_MenuInputPausePending;
int g_MenuMemCardConfirmPending;

void Menu_ResetStateFlags(void) {
    g_MenuLayoutLocked = 0;
    g_MenuItemUseMode = 0;
    g_MenuItemSelectFlag = 0;
    g_MenuSelectionLocked = 0;
    g_MenuActionSubmenuOpen = 0;
    g_MenuEquipMode = 0;
    g_MenuItemRenameMode = 0;
    g_MenuItemDiscardMode = 0;
    g_MenuInputPausePending = 0;
    g_MenuMemCardConfirmPending = 0;
}
