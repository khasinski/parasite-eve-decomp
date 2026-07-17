typedef short s16;

extern int D_800E27EC;
extern char D_800E17E0;

int rand(void);
void func_800CF3AC(void *arg0, void *arg1, int arg2);
void func_800D1DEC(void *arg0, void *arg1, int arg2, int arg3);

int func_800D70C0(int mode, s16 *state) {
    s16 sp10[3];
    char sp18[8];

    switch (mode) {
    case 1:
        state[0] += (rand() & 7) - 3;
        state[1] += (rand() & 3) - 7;
        state[2] += (rand() & 7) - 3;

        if (D_800E27EC < 0x12) {
            break;
        }
        return 1;
    case 2:
        sp10[0] = state[0];
        sp10[1] = state[1];
        sp10[2] = state[2];
        func_800CF3AC(&D_800E17E0, sp18, D_800E27EC);
        func_800D1DEC(sp10, sp18, 0x80, 1);
        break;
    }

    return 0;
}
