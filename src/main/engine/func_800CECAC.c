int GetTPage(int tp, int abr, int x, int y);

extern short D_800E2850[];

void func_800CECAC(void) {
    int i;

    for (i = 0; i < 2; i++) {
        *(short *)((char *)D_800E2850 + i * 4) = GetTPage(i, 0, 0x380, 0x100);
    }

    for (i = 0; i < 2; i++) {
        *(short *)((char *)D_800E2850 + i * 4 + 2) = GetTPage(i, 0, 0x340, 0x100);
    }
}
