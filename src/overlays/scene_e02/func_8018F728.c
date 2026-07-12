typedef signed short s16;

extern void func_800C4E50(void *arg0);

void func_8018F728(void *arg0, void *arg1, char *arg2) {
    *(s16 *)(arg2 + 0x120) = 0x5DC;
    *(s16 *)(arg2 + 0x122) = 0xFF;
    *(s16 *)(arg2 + 0x114) = 0x10;
    *(arg2 + 0x10C) = 0x40;
    *(arg2 + 0x10D) = 0x20;
    *(arg2 + 0x10E) = 0x10;
    *(arg2 + 0x110) = 0x80;
    *(arg2 + 0x111) = 0x80;
    *(arg2 + 0x112) = 0x80;
    *(s16 *)(arg2 + 0x116) = 0xF0;
    *(s16 *)(arg2 + 0x118) = 0xA;
    *(s16 *)(arg2 + 0x124) = 0;
    *(s16 *)(arg2 + 0x11A) = 0;
    *(char **)(arg2 + 0x108) = arg2 + 0x8;
    func_800C4E50(arg2 + 0x108);
}
