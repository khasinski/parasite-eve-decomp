typedef short s16;

extern int D_800E27EC;
extern char D_800E1CC8;
extern char D_800E2224;

void func_800CF3AC(void *arg0, void *arg1, int arg2);
void func_800DB25C(void *arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, void *arg7);

int func_800DB5F4(int mode, s16 *state) {
    char sp20[8];
    int fade;

    switch (mode) {
    case 1:
        if (D_800E27EC < 8) {
            break;
        }
        return 1;
    case 2:
        fade = 0x80 - (D_800E27EC << 4);
        func_800CF3AC(&D_800E1CC8, sp20, state[5]);
        func_800DB25C(&D_800E2224, state[0], state[1], state[2], state[3], 1, fade, sp20);
        break;
    }

    return 0;
}
