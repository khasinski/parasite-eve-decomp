/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

unsigned int g_MenuSelectedItemList;

void Inv_BuildStorageDisplay(void);
void MenuWidget_NavScrollTo(int arg0);
void *MenuWidget_FindByModeAndSelectedBase(int arg0, int arg1);
void MenuWidget_SetCurrentNode(void *arg0);
int Inv_RestoreSelection(unsigned int arg0);
void Menu_StepInventoryRoot(int arg0, int arg1, int arg2);
void Menu_PlayCancelSound(void);

void Menu_InitCategoryTabs(void) {
    void *node;

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
}
