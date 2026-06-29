/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

void Menu_DrawBlendColorChannelRow(void);
void MenuWidget_DrawList(int arg0, void (*arg1)(void));

void Menu_DrawBlendColorChannelList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawBlendColorChannelRow);
}
