#include "scene_e08.h"

extern u16 D_8009D1CC;
extern s32 D_8009D248;
extern s16 D_800942EC;

int func_8001CAB0(int, int, int, int);
int func_80071A54(void);
int func_80077CF4(int);
int func_800C6B90(s16 *, int);

void func_801946D4(void *unused, char *control, char *state)
{
    char *owner = (char *)func_800C2B50();
    /* Keeps the two descending seed-copy loops based at their original offsets. */
    register char *cursor asm("$6");
    s16 point[3];
    int count;
    int random;
    int x;
    int y;
    int z;
    int nextX;
    s32 xVelocity;
    s32 zBase;
    s32 zVelocity;
    s32 yBase;
    int nextY;
    register s32 yVelocity asm("$7");
    int nextZ;
    int zArg;
    /* The Y velocity and later screen limit share the original argument register. */
    register u16 screenLimit asm("$7");
    int pointValue;
    s16 *ground;
    count = 4;
    cursor = state + 0x20;
    for (; count != 0; --count, cursor -= 8)
    {
        *((RoomFxSeed8 *) (cursor + 0x30)) = *((RoomFxSeed8 *) (cursor + 0x28));
        *((RoomFxSeed8 *) (cursor + 0x58)) = *((RoomFxSeed8 *) (cursor + 0x50));
    }

    count = 7;
    x = *((s16 *) (state + 2));
    y = *((s16 *) (state + 6));
    z = *((s16 *) (state + 10));
    cursor = state + 0x38;
    *((s16 *) (state + 0x30)) = x;
    *((s16 *) (state + 0x32)) = y;
    *((s16 *) (state + 0x34)) = z;
    *((RoomFxSeed8 *) (state + 0x58)) = *((RoomFxSeed8 *) (state + 0x20));
    for (; count != 0; --count, cursor -= 8)
    {
        *((RoomFxSeed8 *) (cursor + 0x86)) = *((RoomFxSeed8 *) (cursor + 0x7E));
    }

    *((RoomFxSeed8 *) (state + 0x86)) = *((RoomFxSeed8 *) (state + 0x50));
    /* The volatile reads preserve the retail load order. */
    zBase = *((s32 *)(state + 8));
    zVelocity = *((s32 *)(state + 24));
    xVelocity = *((volatile s32 *)(state + 16));
    yBase = *((volatile s32 *)(state + 4));
    yVelocity = *((volatile s32 *)(state + 20));
    nextZ = zBase + zVelocity;
    nextY = yBase + yVelocity;
    nextX = *((s32 *) state) + xVelocity;
    screenLimit = D_8009D1CC;
    *((s32 *) state) = nextX;
    *((s32 *) (state + 4)) = nextY;
    *((s32 *) (state + 8)) = nextZ;
    zArg = (unsigned)(nextZ >> 16) << 16;
    if (func_8001CAB0(((unsigned)*((s16 *) (state + 2))) << 16, zArg, D_8009D248, screenLimit) == 0)
    {
        *((u8 *) (control + 1)) = 2;
    }
    if ((*((s16 *) (control + 2))) < 50)
    {
        random = func_80071A54();
        *((s16 *) (state + 0x22)) = (random % 1024) - 0x200;
        ground = &D_800942EC;
        *((s16 *) (state + 0x20)) = 0x400;
        *((u16 *) (state + 0x24)) += 0x270;
        if ((*((s32 *) (state + 4))) < (*ground))
        {
            int gravity;
            gravity = 0xC350;
            *((s32 *) (state + 0x14)) += gravity;
        }
        else
        {
            /* Preserve the second ground-height read after clearing velocity. */
            PE1_COMPILER_MEMORY_BARRIER();
            *((s32 *) (state + 0x10)) = 0;
            *((s32 *) (state + 0x14)) = 0;
            *((s32 *) (state + 0x18)) = 0;
            *((s32 *) (state + 4)) = *ground;
        }
    }
    else
    {
        *((s16 *) (state + 0x20)) = 0x400;
        *((s16 *) (state + 0x22)) = 0;
        ++(*((s32 *) (state + 0x2C)));
        *((u16 *) (state + 0x24)) += 0x270;
        *((s32 *) (state + 0x10)) -= (*((u16 *) (state + 0x2A))) * 8;
        *((s32 *) (state + 0x18)) = func_80077CF4((*((s32 *) (state + 0x2C))) << 6) * 1500;
    }
    pointValue = *((s16 *) (state + 2));
    point[0] = pointValue;
    pointValue = *((s16 *) (state + 6));
    point[1] = pointValue;
    pointValue = *((s16 *) (state + 10));
    point[2] = pointValue;
    if (func_800C6B90(point, 0xC8))
    {
        *((s16 *) (owner + 0x2C)) = 1;
    }
}
