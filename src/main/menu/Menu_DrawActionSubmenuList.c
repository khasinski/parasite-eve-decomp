
void Menu_DrawActionSubmenuItem(int arg0);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_DrawActionSubmenuList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawActionSubmenuItem);
}
