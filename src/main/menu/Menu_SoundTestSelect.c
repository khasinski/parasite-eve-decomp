/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_InvItemUsableFlag;
int g_MenuLayoutLocked;
extern signed char g_AyaEquippedWeaponSlot[];
extern signed char g_AyaEquippedArmorSlot[];

int Battle_IsInputAllowedWrapped(void);
int Sfx_DrawActiveListSlot(int arg0);
int Inv_RestoreSelection(int arg0);
void Draw_SetTextDimmed(int arg0);
void Draw_PrintTextById(int arg0);
void MenuWidget_ClearCursorY(void *arg0);
void *MenuWidget_FindByModeAndSelectedBase(int mode, int selectedBase);
void MenuWidget_SetCurrentNode(void *arg0);
void MenuWidget_SetCursorY(void *arg0);

void Menu_SoundTestSelect(void)
{
    int selected;
    int value;
    void *node;

    if (g_MenuLayoutLocked != 0) {
        selected = Inv_RestoreSelection(0);
    } else {
        Draw_SetTextDimmed(Battle_IsInputAllowedWrapped() == 0);
        if (g_InvItemUsableFlag != 0) {
            value = g_AyaEquippedWeaponSlot[0];
        } else {
            value = g_AyaEquippedArmorSlot[0];
        }
        selected = value;
    }

    if (selected >= 0) {
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 6));
        Sfx_DrawActiveListSlot(selected);
        return;
    }

    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 6));
    node = MenuWidget_FindByModeAndSelectedBase(2, 6);
    if (*(int *)((char *)node + 0x44) >= 0) {
        *(int *)((char *)node + 0x44) = -1;
        node = MenuWidget_FindByModeAndSelectedBase(2, 5);
        *(int *)((char *)node + 0x44) = 0;
        MenuWidget_SetCurrentNode(node);
    }
    Draw_PrintTextById(0x39);
}
