/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef struct MenuWidget MenuWidget;

extern void Menu_OpenItemList(void);
extern void Inv_BuildStorageDisplay(void);
extern void MenuWidget_NavScrollTo(int arg0);
extern MenuWidget *MenuWidget_FindByModeAndSelectedBase(int mode, int selectedBase);
extern void MenuWidget_SetCurrentNode(MenuWidget *node);
extern int Inv_RestoreSelection(int arg0);
extern void Menu_StepInventoryRoot(int arg0, int arg1, int arg2);
extern void Menu_PlayCancelSound(void);

extern unsigned int g_MenuSelectedItemList;

int Menu_InventoryNavigate(MenuWidget *arg0, MenuWidget *arg1, unsigned int flags) {
    int ret;
    MenuWidget *node;

    ret = 0;
    if (flags & 0x10000) {
        Menu_OpenItemList();
        ret = 1;
    } else if (flags & 0x40) {
        Inv_BuildStorageDisplay();
        MenuWidget_NavScrollTo(0x2C);
        MenuWidget_NavScrollTo(0xB);
        MenuWidget_NavScrollTo(0xA);
        MenuWidget_NavScrollTo(5);
        MenuWidget_NavScrollTo(6);
        MenuWidget_NavScrollTo(7);

        node = MenuWidget_FindByModeAndSelectedBase(2, 0);
        if (node == 0) {
            node = MenuWidget_FindByModeAndSelectedBase(2, 0x32);
        }
        MenuWidget_SetCurrentNode(node);
        Menu_StepInventoryRoot(0x33E, -1, Inv_RestoreSelection(g_MenuSelectedItemList < 1));
        Menu_PlayCancelSound();
        ret = 1;
    }

    return ret;
}
