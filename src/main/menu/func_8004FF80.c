
void Menu_DrawBlendOptionItem(int arg0);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_DrawBlendColorOptionList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawBlendOptionItem);
}
