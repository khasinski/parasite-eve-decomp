/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuActionItemData;
int g_MenuActiveWidget;

int Inv_RestoreSelection(int arg0);
int Inv_LookupActiveListData(int arg0);
void Menu_DrawActionCodeItem(void);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_DrawUsableItemActionList2(int arg0) {
    int ptr;

    ptr = Inv_LookupActiveListData(Inv_RestoreSelection(1));
    g_MenuActionItemData = ptr;
    g_MenuActiveWidget = arg0;
    MenuWidget_DrawList(arg0, Menu_DrawActionCodeItem);
}
