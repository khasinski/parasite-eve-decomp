typedef short s16;
typedef unsigned short u16;

void func_8018FC34(int arg0, char *arg1, char *arg2) {
    u16 value;

    *(u16 *)(arg2 + 0x10) = *(u16 *)(arg2 + 0x10) + 0xC8;
    *(u16 *)(arg2 + 0x12) = *(u16 *)(arg2 + 0x12) - 0x10;
    value = *(u16 *)(arg2 + 0x14) + 1;
    *(u16 *)(arg2 + 0x14) = value;
    if ((s16)value >= 8) {
        arg1[1] = 2;
    }

    if (*(s16 *)(arg2 + 0x12) < 0) {
        *(u16 *)(arg2 + 0x12) = 0;
    }
}
