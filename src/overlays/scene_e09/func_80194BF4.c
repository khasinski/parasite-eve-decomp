extern int D_80194E00;
extern int D_80194E04;

void func_80194BF4(void *arg0, int arg1, int arg2) {
    volatile int *ptr = &D_80194E00;

    ptr[0] = arg1;
    D_80194E04 = arg2;
}
