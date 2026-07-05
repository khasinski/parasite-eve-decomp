
void Menu_DrawMainMenuItem(int arg0);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_DrawMainMenuList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawMainMenuItem);
}
