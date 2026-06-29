#include "pe1/task_anim.h"

typedef unsigned char u8;
typedef unsigned short u16;

void Task_SetObjAnimEntry12(TaskAnimObj *obj, u8 index, int arg2, int arg3, u8 arg4, u16 arg5,
                   u8 arg6, u8 arg7, u8 arg8, u8 arg9, u8 arg10, u8 arg11) {
    u8 *dst = obj->base + ((index << 4) + 0x1C);

    dst[0] = 0;
    dst[1] = arg2;
    dst[2] = arg3;
    dst[3] = arg4;
    *(u16 *)(dst + 0xC) = arg5;
    dst[0xE] = arg10;
    dst[0xF] = arg11;

    (obj->base + (index << 2))[0x7C] = arg6;
    (obj->base + (index << 2))[0x7D] = arg7;
    (obj->base + (index << 2))[0x7E] = arg8;
    (obj->base + (index << 2))[0x7F] = arg9;
}
