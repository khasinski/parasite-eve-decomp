#include "../room_lib/room_lib.h"

typedef struct RoomPlusActive {
    unsigned char pad0[0x268];
    unsigned short h268;
    unsigned short h26A;
    unsigned short h26C;
} RoomPlusActive;

extern volatile short D_80192C34;
extern volatile short D_80192C36;
extern volatile short D_80192C38;
extern volatile int D_80192C3C;

void func_80192A3C(int mode, int a1, int a2, int a3) {
    volatile RoomChanCtx *ctx;
    volatile short *out;

    if (mode == 1) {
        D_80192C34 = a1;
        D_80192C36 = a2;
        D_80192C38 = a3;
    } else {
        D_80192C3C = a1;
    }

    ctx = D_800F32D0;
    out = &D_80192C34;
    *out = ((volatile RoomPlusActive *)ctx->w8)->h268;
    {
        register volatile RoomPlusActive *activeV1 asm("$3");
        register unsigned short mid asm("$3");

        activeV1 = (volatile RoomPlusActive *)ctx->w8;
        mid = activeV1->h26A;
        D_80192C36 = mid;
    }
    ROOMLIB_SCHED_BARRIER();
    {
        register volatile RoomPlusActive *activeV1 asm("$3");
        register unsigned short frame asm("$4");
        register unsigned short last asm("$5");
        register int period asm("$3");

        activeV1 = (volatile RoomPlusActive *)ctx->w8;
        frame = g_FrameCount16;
        last = activeV1->h26C;
        period = 0x76C;
        D_80192C36 = frame;
        D_80192C3C = period;
        D_80192C38 = last;
    }
}
