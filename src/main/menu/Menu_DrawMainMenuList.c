
void Menu_DrawMainMenuItem(void);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_DrawMainMenuList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawMainMenuItem);
}
