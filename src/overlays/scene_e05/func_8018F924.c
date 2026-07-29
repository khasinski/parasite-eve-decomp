#include "pe1/scene_fx.h"

typedef signed short s16;
typedef signed int s32;

void *func_800C2B50(void);

void func_8018F924(void *arg0, void *arg1, SceneFxVecState *out) {
    char *slot;

    slot = func_800C2B50();
    out->x = *(s32 *)(slot + 0x18);
    out->y = *(s32 *)(slot + 0x1C);
    out->z = *(s32 *)(slot + 0x20);
    out->zero = 0;
    out->scale = 0x400;
}
