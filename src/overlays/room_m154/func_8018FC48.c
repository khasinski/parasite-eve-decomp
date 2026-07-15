void *func_800C2B50(void);
int func_80077CF4(int arg0);
int func_80077DC4(int arg0);

void func_8018FC48(void *unused0, void *unused1, char *state) {
    register char *state_reg asm("$20") = state;
    register char *obj asm("$19") = func_800C2B50();
    register unsigned int i asm("$18") = 0;
    register char *out asm("$17") = state_reg;
    register int angle asm("$16");
    register int call_arg asm("$4");

    do {
        int tmp = *(int *)(obj + 0x18);

        angle = i << 9;
        *(short *)(out + 0) = tmp;
        tmp = *(int *)(obj + 0x1C);
        call_arg = angle;
        *(short *)(out + 2) = tmp - 0x64;
        tmp = *(int *)(obj + 0x20);
        *(short *)(out + 4) = tmp;
        i++;
        tmp = func_80077CF4(call_arg);
        call_arg = angle;
        *(short *)(out + 0x80) = tmp;
        *(short *)(out + 0x82) = 0;
        *(short *)(out + 0x84) = func_80077DC4(call_arg);
        out += 8;
    } while (i < 0x10);

    *(short *)(state_reg + 0x102) = 0x80;
    *(short *)(state_reg + 0x100) = 0x400;
    *(short *)(state_reg + 0x108) = -0x3C;
    state_reg[0x105] = 0;
    *(short *)(state_reg + 0x106) = 0x320;
}
