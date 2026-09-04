#include "common.h"
#include "room_lib.h"

extern RoomBlob8 ROOMLIB_BURST_SPAWNER_SEED;
extern int ROOMLIB_BURST_SPAWNER_CALLBACK();

int ROOMLIB_RUN_BURST_SPAWNER_NAME(int mode, int *count) {
    RoomBlob8 blob;
    RoomMsg msg;
    RoomQRec *rec;
    RoomChanCtx *ctx;

    blob = ROOMLIB_BURST_SPAWNER_SEED;
    if (mode == 1) {
        goto mode1;
    }
    if (mode < 2) {
        if (mode == 0) {
            goto mode0;
        }
        goto ret0;
    }
    if (mode == 2) {
        goto mode2;
    }
    goto ret0;

mode0:
    if (D_800E2368->bD != 0) {
        RoomNodeB **nodes = D_800F32D0->w8;
        if (nodes != 0 && *nodes != 0) {
            unsigned char *state = (*nodes)->state;
            if (*state == 1) {
                *state = 2;
            }
        }
    }
    func_800D3F64(0x59E, func_800D3FD8());
    ctx = D_800F33E0;
    *count = 0;
    return func_800CE560(ctx->w8, 0x14, 0x37,
                         ROOMLIB_BURST_SPAWNER_CALLBACK);

mode1:
    func_800CE8F0(D_800F32D0->w8, 0x11, &blob, &msg);
    func_800CE9D4(D_800F32D0->w8, 0, &msg.sub);
    msg.sub.h0 -= 0x200;
    if (D_800E27EC % 3 == mode && *count < 8) {
        rec = func_800CE610(D_800F33E0->w8);
        if (rec != 0) {
            rec->h0 = msg.h0;
            rec->h2 = msg.h2;
            rec->h4 = msg.h4;
            *(short *)&rec->sub[0] = msg.sub.h0;
            *(short *)&rec->sub[2] = msg.sub.h2;
            *(short *)&rec->sub[4] = msg.sub.h4;
            rec->h10 = 0;
            rec->h12 = 0;
        }
        rec = func_800CE610(D_800F33E0->w8);
        if (rec != 0) {
            rec->h0 = msg.h0;
            rec->h2 = msg.h2;
            rec->h4 = msg.h4;
            *(short *)&rec->sub[0] = msg.sub.h0;
            *(short *)&rec->sub[2] = msg.sub.h2;
            *(short *)&rec->sub[4] = msg.sub.h4;
            rec->h10 = 4;
            rec->h12 = 0;
        }
        (*count)++;
    }
    if (D_800E27EC < 8) {
        goto ret0;
    }
    return 2;

mode2:
    D_800F3372 = 0;
    D_800F3374 = 4;
ret0:
    return 0;
}
