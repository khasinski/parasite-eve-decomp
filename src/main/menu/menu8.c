/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

void Menu_DrawEquipInfoOptionRow(void);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

extern int g_InvItemUsableFlag;
extern int g_MenuInventoryViewMode;

extern int MenuWidget_GetChild(void *arg0, int arg1);
extern int MenuWidget_GridCellIndex(int arg0);
extern void Inv_SortInventoryByMode(int arg0, int arg1);
extern void Sort_InventoryItems(int arg0, int arg1);
extern void Menu_OpenInventoryOrSwapView(int arg0);
extern void Menu_PlayConfirmSound(void);
extern void MenuWidget_DestroyNode(void *arg0);
extern void Menu_PlayCancelSound(void);

void Menu_DrawEquipInfoOptionList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawEquipInfoOptionRow);
}

int Menu_EquipSelectInput(void *arg0, unsigned int arg1) {
    int temp;
    void (*handler)(int, int);

    if (arg1 & 0x10000) {
        temp = MenuWidget_GridCellIndex(MenuWidget_GetChild(arg0, 0));
        handler = g_MenuInventoryViewMode ? Sort_InventoryItems : Inv_SortInventoryByMode;
        handler(g_InvItemUsableFlag, temp);
        Menu_OpenInventoryOrSwapView(1);
        Menu_PlayConfirmSound();
    } else if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }

    return 1;
}
