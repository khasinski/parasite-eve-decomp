/* CC1_FLAGS: -fno-schedule-insns */
#include "room_m273.h"
#include "pe1/psyq_gpu.h"

typedef struct {
    GteShortVector *position;
} RoomM273PaletteSpriteState;

int func_8019A398(int mode, RoomM273PaletteSpriteState *state) {
    register int frame asm("$3");
    register int special asm("$3");
    int size, shade, kind, palette;
    u16 clut;
    int sample, sizeSample, sizeOffset, shadeOffset;
    GteRotation *zeroRotation;
    char stack_pad[8];

    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
    } else if (mode == 2) {
        frame = D_800E27EC - 1;
        sizeOffset = (frame << 8) & 0x3F00;
        shadeOffset = (frame << 9) & 0x3E00;
        sizeSample = ((RoomM273TrigEntry *)((char *)D_800966EC + sizeOffset))->low;
        asm volatile("" : : "r"(sizeSample));
        sample = *(int *)((char *)D_800966EC + shadeOffset);
        special = 4;
        asm volatile("" : "=r"(sample) : "0"(sample));
        size = sizeSample * 2 + 4096;
        shade = (s16)sample >> 5;
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == special && D_800F3428) {
            palette += 8;
        } else {
            palette += 4;
        }
        clut = GetClut(0, palette);
        zeroRotation = 0;
        asm volatile("" : "=r"(zeroRotation) : "0"(zeroRotation));
        func_800CEE20(state->position, zeroRotation, size, size, 100,
                       clut, 1, shade, 0);
    }
    return 0;
}
