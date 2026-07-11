int func_800C6C18(void);
int func_800C2B68(void);

void func_80190254(void *arg0, unsigned char *arg1, char *arg2) {
    register char *timer __asm__("a1") = arg2;
    short count = *(short *)(arg2 + 0x2A);

    if (count != 0) {
        *(short *)(arg2 + 0x2A) = count - 1;
    }

    if (*(short *)(timer + 0x2C) == 1) {
        *(short *)(timer + 0x2C) = 0;
        if (*(short *)(timer + 0x2A) == 0) {
            *(short *)(timer + 0x2A) = *(unsigned short *)(timer + 0x28);
            func_800C6C18();
        }
    }

    if (func_800C2B68() == 1) {
        arg1[1] = 2;
    }
}
