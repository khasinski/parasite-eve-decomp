/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

int g_MenuEquipMode;
int g_MenuItemRenameMode;

int MenuWidget_FindByModeAndSelectedBase(int arg0, int arg1);
int MenuWidget_GridCellIndex(int arg0);
int Inv_GetPackedListItem(int arg0);
void MenuWidget_NavScrollTo(int arg0);
void MenuWidget_SetCurrentNode(int arg0);
int MenuWidget_GetCurrentNode(void);
void MenuWidget_ClearCursorY(int arg0);
void Menu_CloseContextHelpPanel(void);
unsigned char *Inv_LookupActiveListData(int arg0);
int Inv_GetActiveListItem(int arg0);
void Menu_CreateItemUsePanel(int arg0);

void Draw_OffsetCursor(int arg0, int arg1);
void Draw_StatePush(void);
void Draw_StatePop(void);
void Draw_SetColor(int arg0);
void Draw_PrintTextById(int arg0);
void Draw_PrintNumberWidth3Unk(int arg0);
int Draw_GetBlendColor(void);

void Menu_ItemUseAction(int arg0, int arg1) {
    int saved;
    unsigned char *ptr;

    if (arg1 != 0) {
        saved = Inv_GetPackedListItem(MenuWidget_GridCellIndex(MenuWidget_FindByModeAndSelectedBase(2, 0xD)));
        MenuWidget_NavScrollTo(0xF);
        MenuWidget_NavScrollTo(0xB);
        MenuWidget_NavScrollTo(0xD);
        MenuWidget_NavScrollTo(0x18);
        MenuWidget_NavScrollTo(0x30);

        if (g_MenuEquipMode != 0) {
            MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0x32));
        } else {
            if (MenuWidget_GetCurrentNode() != 0) {
                MenuWidget_ClearCursorY(*(int *)(MenuWidget_GetCurrentNode() + 4));
            }
            MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
        }

        g_MenuItemRenameMode = 0;
        Menu_CloseContextHelpPanel();
        ptr = Inv_LookupActiveListData(saved);
        ptr[0x14]++;
        Menu_CreateItemUsePanel(Inv_GetActiveListItem(saved));
    }
}

void Menu_DrawBlendColorChannelRow(int arg0) {
    int shift;
    int value;

    Draw_StatePush();
    shift = arg0 << 3;
    Draw_SetColor(0x80 << shift);
    Draw_PrintTextById(arg0 + 0x35);
    Draw_SetColor(0x808080);
    Draw_OffsetCursor(0x0A, 3);
    value = (Draw_GetBlendColor() >> shift) & 0xFF;
    Draw_PrintNumberWidth3Unk((value - 0x20) >> 1);
    Draw_StatePop();
}
