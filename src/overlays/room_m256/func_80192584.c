typedef unsigned char u8;
typedef signed short s16;

void func_80192654(void);

int func_80192584(u8 *arg0) {
    *(void (**)(void))(arg0 + 0xC) = func_80192654;
    arg0[0x1A] = 0;
    *(int *)(arg0 + 0x10) = 0;
    *(s16 *)(arg0 + 0x1C) = 0;
    *(s16 *)(arg0 + 0x20) = 0;
    *(s16 *)(arg0 + 0x26) = 0;
    return 0;
}
