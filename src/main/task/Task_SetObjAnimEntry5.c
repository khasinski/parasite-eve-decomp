#include "pe1/task_anim.h"

typedef unsigned char u8;
typedef unsigned short u16;

void Task_SetObjAnimEntry5(TaskAnimObj *obj, int index, int arg2, int arg3, u8 arg4, u16 arg5) {
    u8 *dst = obj->base + (((u8)index << 4) + 0x1C);

    dst[0] = 0;
    dst[1] = arg2;
    dst[2] = arg3;
    dst[3] = arg4;
    *(u16 *)(dst + 0xC) = arg5;
}
