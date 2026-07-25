extern void *D_800F32D0;
extern unsigned char D_8019AE9A;

extern void func_8006DCE4(int arg0, void *arg1, int arg2, int arg3, int arg4);

int func_80194D54(int mode) {
    unsigned char *obj;
    unsigned char *state;
    unsigned char obj_mode;
    unsigned short frame;
    unsigned short timer;
    unsigned char *data;

    if (mode == 1) {
        goto mode_one;
    }
    if (mode >= 2) {
        goto ret_zero;
    }
    if (mode != 0) {
        goto ret_zero;
    }
    D_8019AE9A = 0;
    goto ret_zero;

mode_one:
    obj = *(unsigned char **)((unsigned char *)D_800F32D0 + 8);
    state = *(unsigned char **)(*(unsigned char **)obj + 0x18);
    frame = *(unsigned short *)(obj + 0x16);
    timer = *(unsigned short *)(obj + 0x1A);

    if (*state == 1) {
        *state = 2;
    }

    obj_mode = *(unsigned char *)(obj + 0xE);
    if (obj_mode == 9) {
        goto mode_nine;
    }
    if (obj_mode != 0x10) {
        goto check_flag;
    }

    if ((unsigned int)(frame - 0x23) < 0x19) {
        D_8019AE9A = 1;
    }
    goto check_flag;

mode_nine:
    if ((short)frame >= 0x21) {
        D_8019AE9A = 1;
    } else if ((short)frame > 0 && ((int)(timer << 16) <= 0)) {
        data = *(unsigned char **)(obj + 0x238);
        func_8006DCE4(0x5CF, *(void **)(*(unsigned char **)obj + 8),
                      *(short *)(data + 0x594), *(short *)(data + 0x598),
                      *(short *)(data + 0x59C));
    }

check_flag:
    if (D_8019AE9A != 0) {
        goto ret_one;
    }

ret_zero:
    return 0;

ret_one:
    return 1;
}
