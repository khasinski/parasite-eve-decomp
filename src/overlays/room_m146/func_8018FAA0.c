extern char *func_800C2B50(void);
extern int func_80077CF4(int angle);
extern int func_80077DC4(int angle);

void func_8018FAA0(void *arg0, void *arg1, char *rec) {
    char *owner;
    unsigned int i;
    char *cur;
    int angle;

    owner = func_800C2B50();
    for (i = 0; i < 0x10; i++) {
        cur = rec + (i * 8);
        angle = i << 9;
        *(short *)(cur + 0x0) = *(int *)(owner + 0x18);
        *(short *)(cur + 0x2) = *(int *)(owner + 0x1C) - 0x64;
        *(short *)(cur + 0x4) = *(int *)(owner + 0x20);
        *(short *)(cur + 0x80) = func_80077CF4(angle);
        *(short *)(cur + 0x82) = 0;
        *(short *)(cur + 0x84) = func_80077DC4(angle);
    }

    *(short *)(rec + 0x102) = 0x80;
    *(short *)(rec + 0x100) = 0x400;
    *(short *)(rec + 0x108) = -0x3C;
    *(unsigned char *)(rec + 0x105) = 0;
    *(short *)(rec + 0x106) = 0x320;
}
