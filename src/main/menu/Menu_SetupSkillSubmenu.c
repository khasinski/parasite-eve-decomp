/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef unsigned char u8;

int g_MenuActionItemData;
int g_MenuActiveWidget;

int MenuWidget_FindByModeAndSelectedBase(int arg0, int arg1);
int MenuWidget_GridCellIndex(int arg0);
void Inv_SelectActiveList(int arg0);
int MenuWidget_GetChild(int *arg0, int arg1);
int Inv_GetPackedListItem(int arg0);
void Inv_RememberSelection(int arg0, int arg1);
int Inv_RestoreSelection(int arg0);
u8 *Inv_LookupActiveListData(int arg0);
void Draw_SetPrimCallback(int arg0, int arg1);
void Menu_DrawActionCodeItem(void);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_SetupSkillSubmenu(int arg0) {
    int node;
    int other;
    u8 *ptr;

    node = MenuWidget_FindByModeAndSelectedBase(1, 7);
    other = MenuWidget_FindByModeAndSelectedBase(2, 0x36);
    Inv_SelectActiveList(MenuWidget_GridCellIndex(other) == 1);

    if (node != 0 && *(int *)(node + 0x48) == 0) {
        other = MenuWidget_GridCellIndex(MenuWidget_GetChild((int *)node, 0));
        if (other >= 0) {
            other = MenuWidget_FindByModeAndSelectedBase(2, 7);
            other = Inv_GetPackedListItem(MenuWidget_GridCellIndex(other));
            Inv_RememberSelection(1, other);
        }
    }

    ptr = Inv_LookupActiveListData(Inv_RestoreSelection(1));
    g_MenuActionItemData = (int)ptr;
    Draw_SetPrimCallback(arg0, ptr[0x14]);
    g_MenuActiveWidget = arg0;
    MenuWidget_DrawList(arg0, Menu_DrawActionCodeItem);
}
