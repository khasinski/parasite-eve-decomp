typedef signed char s8;
typedef signed short s16;

extern int *func_800C2B10(int arg0);
extern void func_800C4E50(void *arg0);

void func_8018FB6C(void *arg0, void *arg1, char *arg2) {
    int *value;
    char *base;

    *(s16 *)(arg2 + 0x13C) = 0;
    *(s16 *)(arg2 + 0x138) = 0;
    *(s16 *)(arg2 + 0x13A) = 0x80;
    *(s16 *)(arg2 + 0x114) = 0x10;
    *(s16 *)(arg2 + 0x11A) = 0;
    *(s16 *)(arg2 + 0x11C) = 0x80;

    value = func_800C2B10(1);
    *(s8 *)(arg2 + 0x110) = *value;
    value = func_800C2B10(2);
    *(s8 *)(arg2 + 0x111) = *value;
    value = func_800C2B10(3);
    *(s8 *)(arg2 + 0x112) = *value;
    value = func_800C2B10(4);
    *(s8 *)(arg2 + 0x10C) = *value;
    value = func_800C2B10(5);
    *(s8 *)(arg2 + 0x10D) = *value;
    value = func_800C2B10(6);
    *(s8 *)(arg2 + 0x10E) = *value;

    base = arg2 + 0x8;
    *(s16 *)(arg2 + 0x116) = 0x4B0;
    *(s16 *)(arg2 + 0x118) = 0x258;
    *(char **)(arg2 + 0x108) = base;
    func_800C4E50(arg2 + 0x108);

    *(s16 *)(arg2 + 0x12C) = 0x10;
    *(s16 *)(arg2 + 0x132) = 0;
    *(s16 *)(arg2 + 0x134) = 0x80;

    value = func_800C2B10(1);
    *(s8 *)(arg2 + 0x128) = *value >> 1;
    value = func_800C2B10(2);
    *(s8 *)(arg2 + 0x129) = *value >> 1;
    value = func_800C2B10(3);
    *(s8 *)(arg2 + 0x12A) = *value >> 1;
    value = func_800C2B10(4);
    *(s8 *)(arg2 + 0x124) = *value >> 1;
    value = func_800C2B10(5);
    *(s8 *)(arg2 + 0x125) = *value >> 1;
    value = func_800C2B10(6);
    *(s8 *)(arg2 + 0x126) = *value >> 1;

    *(s16 *)(arg2 + 0x12E) = 0x4B0;
    *(s16 *)(arg2 + 0x130) = 0x12C;
    *(char **)(arg2 + 0x120) = base;
    func_800C4E50(arg2 + 0x120);
}
