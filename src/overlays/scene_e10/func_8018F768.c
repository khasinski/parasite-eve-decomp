extern void func_800C6C18(int arg0);
extern int func_800C2B68(void);

void func_8018F768(int arg0, char *arg1, char *arg2) {
    short *state = (short *)arg2;

    if (state[0x33] != 0) {
        state[0x33]--;
    }

    if (state[0x34] == 1) {
        int timer = state[0x33];

        state[0x34] = 0;
        if (timer == 0) {
            state[0x33] = *(unsigned short *)(arg2 + 0x64);
            func_800C6C18(arg0);
        }
    }

    if (func_800C2B68() == 1) {
        arg1[1] = 2;
    }
}
