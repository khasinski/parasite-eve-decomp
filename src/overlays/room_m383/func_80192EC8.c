typedef int s32;
typedef short s16;
typedef unsigned short u16;

void func_80192EC8(s32 arg0, char *arg1, char *arg2) {
    char *obj = arg2;
    u16 value;

    value = *(u16 *)(arg2 + 0x30) - 0x96;
    *(u16 *)(arg2 + 0x30) = value;
    if ((s16)value < 0x190) {
        *(u16 *)(arg2 + 0x30) = 0x190;
    }

    if (*(s16 *)(arg1 + 2) >= 8) {
        value = *(u16 *)(obj + 0x34) - 0x10;
        *(u16 *)(obj + 0x34) = value;
        if ((s16)value < 0) {
            *(u16 *)(obj + 0x34) = 0;
            arg1[1] = 2;
        }
    }
}
