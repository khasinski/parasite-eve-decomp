/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuActiveWidget;
extern int g_SavedDrawBlendColor[];

void Menu_DrawSaveSlotEntry(void);
void MenuWidget_DrawList(int arg0, void (*arg1)(void));
void Draw_BlendColor(int arg0);
void Menu_SetSaveSlotBlendBase(int arg0);

void Menu_DrawSaveSlotList(int arg0) {
    g_MenuActiveWidget = arg0;
    MenuWidget_DrawList(arg0, Menu_DrawSaveSlotEntry);
    Draw_BlendColor(g_SavedDrawBlendColor[0]);
    Menu_SetSaveSlotBlendBase(0);
}
