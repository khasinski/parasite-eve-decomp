
void Menu_DrawActionSubmenuItem(void);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_DrawActionSubmenuList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawActionSubmenuItem);
}
