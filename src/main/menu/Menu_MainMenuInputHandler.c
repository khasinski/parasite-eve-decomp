/* CC1_FLAGS: -g3 */

typedef unsigned int u32;

typedef struct MenuWidget MenuWidget;

extern MenuWidget *MenuWidget_GetChild(MenuWidget *arg0, int arg1);
extern int MenuWidget_GridCellIndex(MenuWidget *arg0);
extern void MenuWidget_DestroyNode(MenuWidget *arg0);
extern void Menu_CreateActionSubmenu(MenuWidget *arg0);
extern void func_8004B03C(MenuWidget *arg0);
extern void Menu_OpenBlendColorScreen(MenuWidget *arg0);
extern void Menu_OpenScreenAdjustView(MenuWidget *arg0);
extern void Inv_InitMaxLevelInventory(int arg0);
extern void Menu_CreateBonusPointAllocationView(void);
extern void Menu_PlayConfirmSound(void);
extern void Menu_PlayCancelSound(void);

int Menu_MainMenuInputHandler(MenuWidget *arg0, u32 arg1) {
    MenuWidget *child = MenuWidget_GetChild(arg0, 0);

    if (arg1 & 0x10000) {
        switch (MenuWidget_GridCellIndex(child)) {
        case 0:
            Menu_CreateActionSubmenu(child);
            break;
        case 1:
            func_8004B03C(child);
            break;
        case 2:
            Menu_OpenBlendColorScreen(child);
            break;
        case 3:
            Menu_OpenScreenAdjustView(child);
            break;
        case 4:
        case 5:
            Inv_InitMaxLevelInventory(MenuWidget_GridCellIndex(child) - 4);
            MenuWidget_DestroyNode(arg0);
            Menu_CreateBonusPointAllocationView();
            Menu_PlayConfirmSound();
            break;
        }
        Menu_PlayConfirmSound();
        return 1;
    }

    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_CreateBonusPointAllocationView();
        Menu_PlayCancelSound();
    }
    return 1;
}
