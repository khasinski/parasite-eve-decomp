#include "common.h"

typedef struct SceneE08PositionSample {
    s32 x, y, z, extra;
} SceneE08PositionSample;

extern s32 D_8009D248;
extern u16 D_8009D1CC;

void *func_800C2B50(void);
int func_8001CAB0(int, int, int, int);
int func_800C6B90(void *, int);

void func_80194214(void *unused, char *phase, char *effect) {
    char *owner;
    s16 point[3];
    int i;
    int angle;
    u16 preserved;
    s32 x, z;

    owner = func_800C2B50();
    for (i = 2; i != 0; i--) {
        __builtin_memcpy(effect + 0x30 + i * 8, effect + 0x28 + i * 8, 8);
        *(SceneE08PositionSample *)(effect + i * 0x10) =
            *(SceneE08PositionSample *)(effect + (i - 1) * 0x10);
        __builtin_memcpy(effect + 0x48 + i * 8, effect + 0x40 + i * 8, 8);
    }

    if (*(s16 *)(phase + 2) >= 0x33) {
        angle = *(u16 *)(effect + 0x8A) + *(u16 *)(effect + 0x8C);
        preserved = *(u16 *)(effect + 0x34);
        *(u16 *)(effect + 0x8A) = angle;
        *(u16 *)(effect + 0x34) = preserved;
        *(s32 *)effect = *(s32 *)effect - (s16)angle;
    }
    if ((u16)(*(u16 *)(phase + 2) - 0x22) < 9) {
        *(u16 *)(effect + 0x48) += 0x12C;
    }
    if (*(s16 *)(phase + 2) >= 0x4C) {
        *(u16 *)(effect + 0x4A) += 0x96;
    }
    if (*(s16 *)(phase + 2) == 0x78) {
        *(u8 *)(phase + 1) = 2;
    }

    x = *(s32 *)effect;
    point[0] = x;
    point[1] = *(s32 *)(effect + 4);
    z = *(s32 *)(effect + 8);
    point[2] = z;
    if (func_8001CAB0((u32)x << 16, (u32)z << 16, D_8009D248, D_8009D1CC) == 0) {
        *(u8 *)(phase + 1) = 2;
    }
    if (func_800C6B90(point, 0x320) != 0) {
        *(s16 *)((char *)owner + 0x2C) = 1;
    }
}
