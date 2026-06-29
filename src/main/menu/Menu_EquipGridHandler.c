/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuInventoryViewMode;

int MenuWidget_GetChild(int arg0, int arg1);
int MenuWidget_GridCellIndex(int arg0);
void Sort_ContainerItems(int arg0);
void Inv_SortInventoryByMode(int arg0, int arg1);
void Menu_OpenInventoryOrSwapView(int arg0);
void Menu_PlayConfirmSound(void);
void MenuWidget_DestroyNode(int arg0);
void Menu_PlayCancelSound(void);

int Menu_EquipGridHandler(int arg0, int arg1) {
    int temp;

    if (arg1 & 0x10000) {
        temp = MenuWidget_GridCellIndex(MenuWidget_GetChild(arg0, 0));
        if (g_MenuInventoryViewMode != 0) {
            Sort_ContainerItems(temp);
        } else {
            Inv_SortInventoryByMode(2, temp);
        }
        Menu_OpenInventoryOrSwapView(1);
        Menu_PlayConfirmSound();
    } else if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }

    return 1;
}
