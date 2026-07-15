extern int D_80194E00;
extern int D_80194E04;

void func_80194BF4(int arg0, int arg1, int arg2) {
    register int *dst asm("$2") = &D_80194E00;

    __asm__ volatile("" : "=r"(dst) : "0"(dst));
    dst[0] = arg1;
    D_80194E04 = arg2;
}
