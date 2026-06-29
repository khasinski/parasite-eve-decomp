asm(".globl func_80062A20");
asm("func_80062A20 = MenuWidget_GetChild");

int MenuWidget_GetChild(int *arg0, int arg1) {
    return arg0[arg1 + 2];
}
