void Menu_SoundTestSelect(void);
void MenuWidget_DrawList(void *arg0, void (*arg1)(void));

asm(".globl func_8004F978");
asm("func_8004F978 = Menu_DrawSoundTestList");

void Menu_DrawSoundTestList(void *arg0) {
    MenuWidget_DrawList(arg0, Menu_SoundTestSelect);
}
