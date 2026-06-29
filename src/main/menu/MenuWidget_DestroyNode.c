void MenuWidget_DestroyNodeRecursive(void);

asm(".globl func_80062F1C");
asm("func_80062F1C = MenuWidget_DestroyNode");

void MenuWidget_DestroyNode(void) {
    MenuWidget_DestroyNodeRecursive();
}
