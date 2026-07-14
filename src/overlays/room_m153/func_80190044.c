typedef unsigned short u16;

char *func_800C2B50(void);

void func_80190044(void *arg0, void *arg1, char *work) {
    register int i asm("$4");
    register char *cur asm("$3");
    char *src;
    int value;

    src = func_800C2B50();
    i = 0;
    cur = work;
    do {
        value = *(int *)(src + 0x18);
        *(u16 *)(cur + 0) = value;
        value = *(int *)(src + 0x1C);
        *(u16 *)(cur + 2) = value;
        value = *(int *)(src + 0x20);
        i++;
        *(u16 *)(cur + 4) = value;
        cur += 8;
    } while ((unsigned int)i < 0x10);

    *(u16 *)(work + 0x82) = 0x80;
    *(u16 *)(work + 0x80) = 0x800;
}
