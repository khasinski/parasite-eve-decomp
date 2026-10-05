extern void func_800C6C18(int arg0);
extern int func_800C2B68(void);

void func_801905C8(int arg0, char *arg1, char *arg2) {
    short *state = (short *)arg2;

    if (state[3] != 0) {
        state[3]--;
    }

    if (state[4] == 1) {
        int timer = state[3];

        state[4] = 0;
        if (timer == 0) {
            state[3] = *(unsigned short *)(arg2 + 4);
            func_800C6C18(arg0);
        }
    }

    if (func_800C2B68() == 1) {
        arg1[1] = 2;
    }
}
