#include "common.h"
/* MASPSX_FLAGS: --expand-div */

extern char *g_AkaoCurTrack;

static inline u8 **SetDuration(u8 **cursor) {
    u8 *pc = *cursor;
    char *track;
    int value;

    *cursor = pc + 1;
    track = g_AkaoCurTrack;
    value = pc[0];
    *(u16 *)(track + 0x58) = value;
    if (value == 0) {
        *(u16 *)(track + 0x58) = 0x100;
    }
    return cursor;
}

void SeqOp_SetVolumeSlideTarget(void *ptr) {
    void *stream;
    u8 *pc;
    int value;

    stream = SetDuration((u8 **)ptr);

    {
        int high;
        char *track;
        int current;

        pc = *(u8 **)stream;
        *(u8 **)stream = pc + 1;
        value = pc[0];
        *(u8 **)stream = pc + 2;
        high = pc[1];

        track = g_AkaoCurTrack;
        value <<= 16;
        high <<= 24;
        value |= high;

        current = *(u32 *)(track + 0x40);
        current &= 0xFFFF0000;
        *(u32 *)(track + 0x40) = current;
        *(u32 *)(track + 0x44) = (value - current) / *(u16 *)(track + 0x58);
    }
}
