/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

void Menu_DrawBlendColorOptionRow(void);
void MenuWidget_DrawList(int arg0, void (*arg1)(void));

void Menu_DrawBlendColorOptionList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawBlendColorOptionRow);
}
