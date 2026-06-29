/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int D_8009CFF4;

void *MenuWidget_FindByModeAndSelectedBase(int arg0, int arg1);
void *MenuWidget_CreateSimpleNode(int arg0, int arg1, int arg2, int arg3);
void Menu_DrawParasiteSpellInfoPanel(void);

void Menu_CreateParasiteSpellInfoPanel(int arg0) {
    void *widget;

    if (MenuWidget_FindByModeAndSelectedBase(1, 0x1E) == 0) {
        widget = MenuWidget_CreateSimpleNode(0x1E, 0, 0, 0);
        *(void **)((char *)widget + 0x30) = Menu_DrawParasiteSpellInfoPanel;
        MenuWidget_CreateSimpleNode(0x1C, 0, 0, 0);
    }

    D_8009CFF4 = arg0;
}
