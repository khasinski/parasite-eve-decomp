/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef unsigned char u8;

int g_MenuActionItemData;
int g_InvItemUsableFlag;
int g_MenuActiveWidget;

int Inv_RestoreSelection(int arg0);
u8 *Inv_LookupActiveListData(int arg0);
void Draw_SetPrimCallback(int arg0, int arg1);
void Menu_DrawActionCodeItem(void);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_DrawUsableItemActionList(int arg0) {
    u8 *ptr;

    ptr = Inv_LookupActiveListData(Inv_RestoreSelection(0));
    g_MenuActionItemData = (int) ptr;
    g_InvItemUsableFlag = ptr[6] != 9;
    Draw_SetPrimCallback(arg0, ptr[0x14]);
    g_MenuActiveWidget = arg0;
    MenuWidget_DrawList(arg0, Menu_DrawActionCodeItem);
}
