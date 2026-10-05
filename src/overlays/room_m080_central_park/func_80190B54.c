#include "../room_lib/RoomSharedGlobalState.h"

extern char *func_800C2B50(void);
extern int func_800C5EB0(void *obj, s16 *values, int *result);
extern RoomSharedGlobalState *D_8009D254;

void func_80190B54(void *arg0, char *state, char *sys) {
    char *statep = state;
    char *sysp = sys;
    int *resultp;
    char *root;
    RoomSharedGlobalState *global;
    int value0;
    int value1;
    int value2;
    s16 values[3];
    int result;
    int ret;

    root = func_800C2B50();
    resultp = &result;
    global = D_8009D254;
    value0 = global->field2A;
    values[0] = value0;
    value1 = global->field2A;
    values[1] = value1;
    value2 = global->field2A;
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
