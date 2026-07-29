#include "pe1/scene_fx.h"

typedef signed short s16;
typedef signed int s32;
typedef unsigned int u32;

void *func_800C2B50(void);

void func_8018FE7C(void *arg0, void *arg1, SceneFxVecTable16 *out) {
    char *slot;
    u32 i;

    slot = func_800C2B50();
    for (i = 0; i < 16; i++) {
        out->entries[i].x = *(s32 *)(slot + 0x18);
        out->entries[i].y = *(s32 *)(slot + 0x1C);
        out->entries[i].z = *(s32 *)(slot + 0x20);
    }
    out->count = 0x80;
    out->scale = 0x800;
}
