typedef unsigned char u8;
typedef signed short s16;

int func_801925AC(u8 *arg0, int arg1, int arg2, int arg3, int arg4, int arg5) {
    if (arg2 == 0x100) {
        goto cmd_100;
    }
    if ((unsigned int)arg2 >= 0x101) {
        goto high_cmd;
    }
    if (arg2 == 0x19) {
        goto cmd_19;
    }
    goto done;

high_cmd:
    if (arg2 == 0x101) {
        goto cmd_101;
    }
    goto done;

cmd_19:
    if (arg1 != 1) {
        goto done;
    }
    *(int *)(arg0 + 0x10) = arg3;
    *(int *)arg3 = arg1;
    goto done;

cmd_100:
    *(s16 *)(arg0 + 0x22) = arg3;
    *(s16 *)(arg0 + 0x24) = arg4;
    goto done;

cmd_101:
    *(s16 *)(arg0 + 0x28) = arg3;
    *(s16 *)(arg0 + 0x2A) = arg4;
    *(s16 *)(arg0 + 0x2C) = arg5;

done:
    return 0;
}
