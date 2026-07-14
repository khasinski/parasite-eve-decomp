typedef unsigned char u8;
typedef signed short s16;
typedef int s32;
typedef unsigned int u32;

extern void D_8019838C(void);

s32 func_801982CC(char *arg0, s32 arg1, u32 arg2, char *arg3, s32 arg4, s32 arg5) {
    register s32 value asm("$2");
    register s32 stack0 asm("$8");
    register s32 stack1 asm("$3");

    stack0 = arg4;
    stack1 = arg5;
    value = 0xA;
    if (arg2 == value) {
        goto case10;
    }
    asm volatile("" ::: "memory");

    if (arg2 >= 0xB) {
        goto ge11;
    }

    value = 4;
    if (arg2 == value) {
        goto case4;
    }
    goto store;

ge11:
    value = 0x19;
    if (arg2 == value) {
        goto case25;
    }
    value = 0x1C;
    if (arg2 == value) {
        goto case28;
    }
    goto store;

case25:
    value = 1;
    if (arg1 != value) {
        goto done;
    }
    *(char **)(arg0 + 0x10) = arg3;
    asm volatile("" : "=r"(arg1) : "0"(arg1));
    *(s32 *)arg3 = arg1;
    goto store;

case4:
    *(char **)(arg0 + 0x3C) = arg3;
    *(s32 *)(arg0 + 0x40) = stack0;
    *(s16 *)(arg0 + 0x44) = stack1;
    goto store;

case28:
    *(s16 *)(arg0 + 0x48) = (s32)arg3;
    *(s16 *)(arg0 + 0x46) = stack0;
    goto store;

case10:
    value = (s32)D_8019838C;

store:
    asm volatile("" ::: "memory");
    arg0[0x16] = (s32)arg3;
    *(s32 *)(arg0 + 0xC) = value;
    asm volatile("" ::: "memory");
done:
    value = 0;
    return value;
}
