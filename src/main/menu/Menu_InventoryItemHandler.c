/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuSelectedItemIndex;
int D_8009CFBC;

int MenuWidget_GetChild(int arg0, int arg1);
int MenuWidget_GetCellIndex(int arg0);
void Menu_CreateItemActionSubmenu(void);
void Menu_CreateItemList(void);
void Menu_PlayConfirmSound(void);
void Menu_PlayErrorSound(void);
void MenuWidget_DestroyNode(int arg0);
void Menu_PlayCancelSound(void);

int Menu_InventoryItemHandler(int arg0, int arg1) {
    int temp;

    if (arg1 & 0x10000) {
        temp = MenuWidget_GetCellIndex(MenuWidget_GetChild(arg0, 0));
        g_MenuSelectedItemIndex = temp;
        if (temp >= 0) {
            if ((temp & 1) || (D_8009CFBC == 0)) {
                Menu_CreateItemActionSubmenu();
            } else {
                Menu_CreateItemList();
            }
            Menu_PlayConfirmSound();
        } else {
            Menu_PlayErrorSound();
        }
    }

    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }

    return 1;
}
