typedef short s16;

typedef struct RoomGlobalState {
    char pad0[0x2A];
    s16 h2A;
} RoomGlobalState;

extern char *func_800C2B50(void);
extern int func_800C5EB0(void *obj, s16 *values, int *result);
extern RoomGlobalState *D_8009D254;

void func_80190B54(void *arg0, char *state, char *sys) {
    register char *statep asm("s3") = state;
    register char *sysp asm("s1") = sys;
    register int *resultp asm("s0");
    register char *root asm("s2");
    register RoomGlobalState *global asm("v1");
    register int value0 asm("a1");
    register int value1 asm("a2");
    register int value2 asm("v1");
    s16 values[3];
    int result;
    int ret;

    root = func_800C2B50();
    resultp = &result;
    global = D_8009D254;
    value0 = global->h2A;
    values[0] = value0;
    value1 = global->h2A;
    values[1] = value1;
    value2 = global->h2A;
    values[2] = value2;

    func_800C5EB0(sysp + 0xAC, values, resultp);
    ret = func_800C5EB0(sysp + 0x78, values, resultp);

    if (result == 1) {
        *(s16 *)(root + 0x68) = 1;
    }

    if (ret == 1) {
        statep[1] = 2;
    }
}
