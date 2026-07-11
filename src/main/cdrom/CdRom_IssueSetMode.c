extern int D_8009B598;

int CdRom_SendCmd(int arg0);

int func_8007FCBC(int arg0) {
    register volatile int *ptr asm("$3");

    ptr = &D_8009B598;
    if (*ptr > 0) {
        return 0;
    }
    asm volatile("" : "=r"(ptr) : "0"(ptr));
    ptr[-10] = 0x20;
    return CdRom_SendCmd(arg0 & 0xFF);
}
