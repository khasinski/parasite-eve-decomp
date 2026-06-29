void Menu_DrawActionOptionRow(void) __asm__("func_800509A8");
void MenuWidget_DrawList(void *arg0, void (*arg1)(void));

asm(".globl func_8004F950");
asm("func_8004F950 = Menu_DrawActionOptionList");

void Menu_DrawActionOptionList(void *arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawActionOptionRow);
}
