
void Menu_DrawBlendColorChannelRow(int arg0);
void MenuWidget_DrawList(int arg0, void (*arg1)(void));

void Menu_DrawBlendColorChannelList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawBlendColorChannelRow);
}
