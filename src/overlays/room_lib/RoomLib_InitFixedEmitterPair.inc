#include "common.h"
#include "pe1/room_fx.h"

typedef struct RoomFxInitBounds {
    char pad0[0x10];
    s16 width;
    s16 height;
    s16 alpha;
    s16 depth;
} RoomFxInitBounds;

extern char ROOMLIB_FIXED_PAIR_SOURCE[];
extern u8 ROOMLIB_FIXED_PAIR_PRIMARY_COLOR0_0;
extern u8 ROOMLIB_FIXED_PAIR_PRIMARY_COLOR0_1;
extern u8 ROOMLIB_FIXED_PAIR_PRIMARY_COLOR0_2;
extern u8 ROOMLIB_FIXED_PAIR_PRIMARY_COLOR1_0;
extern u8 ROOMLIB_FIXED_PAIR_PRIMARY_COLOR1_1;
extern u8 ROOMLIB_FIXED_PAIR_PRIMARY_COLOR1_2;
extern s16 ROOMLIB_FIXED_PAIR_PRIMARY_MODE;
extern s16 ROOMLIB_FIXED_PAIR_PRIMARY_EXTENT0;
extern s16 ROOMLIB_FIXED_PAIR_PRIMARY_EXTENT1;
extern s16 ROOMLIB_FIXED_PAIR_PRIMARY_OFFSET;
extern s16 ROOMLIB_FIXED_PAIR_PRIMARY_INTENSITY;

void func_800C4E50(RoomFxEmitterParams *params);

void ROOMLIB_INIT_FIXED_PAIR_NAME(int unused0, int unused1,
                                  RoomFxInitBounds *bounds) {
    s16 *anchor;
    char *source;
    int mode;
    register int intensity asm("$19");
    int color1_0;
    register int color1_1 asm("$22");
    register int color1_2 asm("$20");
    register int extent1 asm("$18");
    RoomFxEmitterParams *params;
    RoomFxEmitterParams *secondary;
    int extent0;

    anchor = &ROOMLIB_FIXED_PAIR_PRIMARY_MODE;
    params = (RoomFxEmitterParams *)(anchor - 6);

    bounds->width = 10;
    bounds->height = 10;
    bounds->depth = 20;
    bounds->alpha = 255;

    mode = 16;
    intensity = 128;
    color1_0 = 120;
    color1_1 = 100;
    color1_2 = 30;
    asm volatile("" ::: "memory");
    extent0 = 0x6A4;
    extent1 = 0x514;
    asm volatile("" ::: "memory");
    source = ROOMLIB_FIXED_PAIR_SOURCE;

    *anchor = mode;
    ROOMLIB_FIXED_PAIR_PRIMARY_OFFSET = 0;
    ROOMLIB_FIXED_PAIR_PRIMARY_INTENSITY = intensity;
    ROOMLIB_FIXED_PAIR_PRIMARY_COLOR1_0 = color1_0;
    ROOMLIB_FIXED_PAIR_PRIMARY_COLOR1_1 = color1_1;
    ROOMLIB_FIXED_PAIR_PRIMARY_COLOR1_2 = color1_2;
    ROOMLIB_FIXED_PAIR_PRIMARY_COLOR0_0 = 0;
    ROOMLIB_FIXED_PAIR_PRIMARY_COLOR0_1 = 0;
    ROOMLIB_FIXED_PAIR_PRIMARY_COLOR0_2 = 0;
    ROOMLIB_FIXED_PAIR_PRIMARY_EXTENT0 = extent0;
    ROOMLIB_FIXED_PAIR_PRIMARY_EXTENT1 = extent1;
    params->source = source;
    func_800C4E50(params);

    params = (RoomFxEmitterParams *)(anchor + 6);
    extent0 = 0x226;
    asm volatile("" : "=r"(anchor) : "0"(anchor));
    source += 0x100;
    secondary = (RoomFxEmitterParams *)(anchor + 6);
    secondary->mode = mode;
    secondary->offset = 0;
    secondary->intensity = intensity;
    secondary->color1[0] = 0;
    secondary->color1[1] = 0;
    secondary->color1[2] = 0;
    secondary->color0[0] = color1_0;
    secondary->color0[1] = color1_1;
    secondary->color0[2] = color1_2;
    secondary->extent1 = extent0;
    secondary->extent0 = extent1;
    secondary->source = source;
    func_800C4E50(params);
}

#undef ROOMLIB_INIT_FIXED_PAIR_NAME
#undef ROOMLIB_FIXED_PAIR_SOURCE
#undef ROOMLIB_FIXED_PAIR_PRIMARY_COLOR0_0
#undef ROOMLIB_FIXED_PAIR_PRIMARY_COLOR0_1
#undef ROOMLIB_FIXED_PAIR_PRIMARY_COLOR0_2
#undef ROOMLIB_FIXED_PAIR_PRIMARY_COLOR1_0
#undef ROOMLIB_FIXED_PAIR_PRIMARY_COLOR1_1
#undef ROOMLIB_FIXED_PAIR_PRIMARY_COLOR1_2
#undef ROOMLIB_FIXED_PAIR_PRIMARY_MODE
#undef ROOMLIB_FIXED_PAIR_PRIMARY_EXTENT0
#undef ROOMLIB_FIXED_PAIR_PRIMARY_EXTENT1
#undef ROOMLIB_FIXED_PAIR_PRIMARY_OFFSET
#undef ROOMLIB_FIXED_PAIR_PRIMARY_INTENSITY
