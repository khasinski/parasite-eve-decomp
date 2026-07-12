typedef signed short s16;
typedef unsigned short u16;
typedef unsigned char u8;

extern int *D_8009D254;

int func_800C6CE0(void *arg0);
int func_800C2B68(void);

void func_8018F61C(char *arg0, u8 *arg1, char *arg2) {
    register char *obj asm("$17");
    register u8 *state asm("$18");
    register char *work asm("$16");
    register char *store asm("$4");
    register int value asm("$2");
    register int temp asm("$3");
    register int *flags asm("$3");
    int *objectFlags;
    int scratch;

    obj = arg0;
    state = arg1;
    work = arg2;
    __asm__ volatile("" : "=m"(scratch));
    value = *(s16 *)(work + 0x2A);
    store = work;
    temp = value;
    if (value != 0) {
        goto dec_timer;
    }

    if (*(s16 *)(work + 0x2C) != 1) {
        goto after_timer;
    }
    if (func_800C6CE0(obj) != 3) {
        goto after_timer;
    }
    if (*(u8 *)*(int *)(*(int *)(*(int *)(obj + 8)) + 0x18) != 2) {
        goto after_timer;
    }

    value = *(u16 *)(work + 0x28);
    __asm__ volatile("" : "=r"(value) : "0"(value));
    flags = D_8009D254;
    *(s16 *)(work + 0x2C) = 0;
    *(s16 *)(work + 0x2A) = value;
    flags = *(int **)flags;
    value = flags[0x13];
    value |= 0x4000;
    flags[0x13] = value;

    objectFlags = *(int **)*(int **)(obj + 8);
    store = (char *)objectFlags;
    value = *objectFlags;
    value |= 0x80000000;
    *objectFlags = value;
    goto set_timer;

dec_timer:
    value = temp - 1;
set_timer:
    *(s16 *)(store + 0x2A) = value;
    *(s16 *)(store + 0x2C) = 0;

after_timer:
    if (func_800C2B68() == 1) {
        state[1] = 2;
    }
}
