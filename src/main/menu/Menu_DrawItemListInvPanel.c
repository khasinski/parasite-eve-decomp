
void Menu_DrawInvPanelItemStub(void);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_DrawItemListInvPanel(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawInvPanelItemStub);
}
