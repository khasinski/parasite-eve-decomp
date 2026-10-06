#include "common.h"
extern char D_8018FFD0;
extern char D_8018FFA8;
extern char D_8018FFE4;

s32 func_800C251C(void *arg0, void *arg1);
s32 func_800C2758(void *arg0, void *arg1, void *arg2);
int FieldEng_GetStatus(void);
#include "pe1/room_module.h"

s32 func_8018F0BC(void *arg0) {
    s32 ret;

    if (FieldEng_GetStatus() == 3) {
        ret = func_800C251C(arg0, &D_8018FFD0);
        ret = ret | func_800C2758(arg0, &D_8018FFA8, &D_8018FFE4);
    } else {
        ret = -1;
    }

    if (ret == -1) {
        RoomLib_CloseTarget(arg0);
    }

    return 0;
}
