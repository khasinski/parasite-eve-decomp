typedef unsigned char u8;
typedef signed short s16;
typedef int s32;

extern void **D_8009D254;

void func_80020CE4(void);
void func_80192C00(void *arg0, void *arg1);

s32 func_80192D8C(char *arg0) {
    char *link;
    char *target;
    char *state;

    target = **(char ***)(arg0 + 8);
    link = arg0 + 0xC;
    if (target != 0) {
        state = *(char **)(target + 0x18);
        state[0] = 4;
    }

    **(s32 ***)(arg0 + 0x10) = 0;
    arg0[0] = 4;
    if (arg0[0x4B] != 0) {
        target = *D_8009D254;
        if (target != 0) {
            if (*(s16 *)(target + 0xC) > 0) {
                func_80020CE4();
            }
        }
        func_80192C00(*(void **)(arg0 + 8), link);
    }

    return 0;
}
