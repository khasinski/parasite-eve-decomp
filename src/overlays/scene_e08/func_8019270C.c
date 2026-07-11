typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned short u16;

void func_8019270C(void *arg0, char *arg1, char *arg2) {
    register s32 current __asm__("v0");
    register s32 original __asm__("v1");
    s16 value;
    register char *state __asm__("a0");

    __asm__ volatile("addiu $sp,$sp,-8");
    current = *(s16 *)(arg2 + 0x10);
    original = current;
    __asm__ volatile("" : "=r"(current) : "0"(current));
    state = arg2;
    if (current < 0x960) {
        current = original + 0x28;
        *(s16 *)(arg2 + 0x10) = current;
    }

    if (*(s16 *)(arg1 + 0x2) >= 0x3D) {
        value = *(u16 *)(state + 0x12) + 0x190;
        *(s16 *)(state + 0x12) = value;
        if (value >= 0x1449) {
            *(s16 *)(state + 0x12) = 0x1448;
        }
    }

    if (*(s16 *)(arg1 + 0x2) >= 0x6A) {
        *(u8 *)(arg1 + 0x1) = 2;
    }
    __asm__ volatile("addiu $sp,$sp,8");
}
