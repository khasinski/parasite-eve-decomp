asm(".globl Util_ReturnTrue");
asm("Util_ReturnTrue = func_8003D82C");

int Util_ReturnTrue(void *unused) __asm__("func_8003D82C");

int Util_ReturnTrue(void *unused) {
    return 1;
}
