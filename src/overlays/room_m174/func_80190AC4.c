typedef int s32;
typedef short s16;
typedef unsigned short u16;

void func_80190AC4(s32 arg0, char *arg1, char *arg2) {
    char *obj = arg2;
    u16 value;

    if (*(s16 *)(arg2 + 0x14) == 1) {
        *(u16 *)(arg2 + 0x10) += 0xC8;
    }

    if (*(s16 *)(arg2 + 0x14) == 0) {
        *(u16 *)(arg2 + 0x10) -= 0xC8;
    }

    value = *(u16 *)(obj + 0x12) - 0x10;
    *(u16 *)(obj + 0x12) = value;
    if ((s16)value < 0) {
        *(u16 *)(obj + 0x12) = 0;
    }

    if (*(s16 *)(arg1 + 2) >= 0x15) {
        arg1[1] = 2;
    }
}
