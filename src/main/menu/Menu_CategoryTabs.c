/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

unsigned int g_MenuSelectedItemList;

void Inv_BuildStorageDisplay(void);
void MenuWidget_NavScrollTo(int arg0);
void *MenuWidget_FindByModeAndSelectedBase(int arg0, int arg1);
void MenuWidget_SetCurrentNode(void *arg0);
int Inv_RestoreSelection(unsigned int arg0);
void Menu_StepInventoryRoot(int arg0, int arg1, int arg2);
void Menu_PlayCancelSound(void);

int g_MenuEquipMode;

void Draw_OffsetCursor(int arg0, int arg1);
void Draw_FlushPrimList(int arg0);
void Draw_SetColor(int arg0);
void Draw_PrintTextById(int arg0);

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

void Menu_DrawEquipScreenHeader(void) {
    Draw_OffsetCursor(0, 0xA);
    Draw_FlushPrimList(0x69);

    if (g_MenuEquipMode != 0) {
        Draw_SetColor(0x78703C);
        Draw_OffsetCursor(0xA, 0x16);
        Draw_PrintTextById(0x1F);
        Draw_OffsetCursor(0, 0x10);
        Draw_PrintTextById(0x21);
        Draw_SetColor(0x808080);
    }
}
