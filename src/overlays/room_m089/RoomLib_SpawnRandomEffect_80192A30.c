#include "room_m089.h"

int func_80192A30(int mode, unsigned short *dst, unsigned short *src) {
    short *effect;
    RoomChanCtx *channel;
    int random;

    switch (mode) {
    case 0:
    channel = D_800F33E0;
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    return func_800CE560(channel->w8, 0x38, 8, func_801924F8);

    case 1:
    if (D_800E27EC < 9) {
        effect = func_800CE610(D_800F33E0->w8);
        if (effect != 0) {
            effect[2] = 0;
            effect[1] = 0;
            effect[0] = 0;
            effect[6] = 0;
            effect[5] = 0;
            effect[4] = 0;
            effect[10] = 0;
            effect[9] = 0;
            effect[8] = 0;
            effect[14] = 0;
            effect[13] = 0;
            effect[12] = 0;
            effect[18] = 0;
            effect[17] = 0;
            effect[16] = 0;
            effect[22] = 0;
            effect[21] = 0;
            effect[20] = 0;
            effect[25] = 0;
            effect[24] = 0;
            effect[26] = func_80071A54();
            effect[3] = (func_80071A54() & 0x3FF) + 0x400;
            if (func_80071A54() & 1) {
                effect[7] = (func_80071A54() & 0x7F) + 0x1AC;
            } else {
                effect[7] = -0x1AC - (func_80071A54() & 0x7F);
            }
            effect[27] = (func_80071A54() & 0x3FF) + 0x5DC;
        }
    }

    if (D_800E27EC == 9) {
        void **sound = &D_800B0E64;
        ROOMLIB_V0_PTR_DECL(current);
        int volume;
        ROOMLIB_LOAD_PTR(current, sound);
        if (current != 0) {
            volume = 0x7F;
            random = func_800D3FD8();
            func_8006DF50(*sound, 0x5BB, random, 0x80, volume);
            ROOMLIB_LOAD_PTR(current, sound);
            if (current != 0) {
                func_8006DF50(*sound, 0x5BC, 0x80, 0x80, volume);
            }
        }
    }
    if (D_800E27EC < 0x24) {
        break;
    }
    return 1;

    case 2:
    {
        ROOMLIB_V1_INT_DECL(value);
        ROOMLIB_A0_INT_DECL(index);
        index = D_800E11FA;
        D_800F3368 = 0x20;
        D_800F336A = 2;
        D_800F3376 = 0x20;
        D_800F3378 = 0x20;
        ROOM_M089_LOAD_EFFECT_LOOKUP(value, index);
        D_800F336C = 3;
        D_800F336E = 1;
        D_800F3372 = 0;
        D_800F3374 = 4;
        D_800F3370 = value;
    }
        break;
    default:
        break;
    }
    return 0;
}
