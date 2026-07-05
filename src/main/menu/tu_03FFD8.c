/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

void *g_MenuSelectedItemData;

void Draw_OffsetCursor(int arg0, int arg1);
void Sfx_CursorRenderData(void *arg0);

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

void Menu_DrawEquipItemIcon(void) {
    Draw_OffsetCursor(6, 6);
    Sfx_CursorRenderData(g_MenuSelectedItemData);
}

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
