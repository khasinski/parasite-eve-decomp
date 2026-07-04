void Menu_SoundTestSelect(void);
void MenuWidget_DrawList(void *arg0, void (*arg1)(void));

void Menu_DrawSoundTestList(void *arg0) {
    MenuWidget_DrawList(arg0, Menu_SoundTestSelect);
}
