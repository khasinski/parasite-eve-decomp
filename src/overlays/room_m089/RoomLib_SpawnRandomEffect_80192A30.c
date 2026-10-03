#include "room_m089.h"
#include "pe1/room_sound_slot.h"

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
        RoomSoundSlot *sound = &D_800B0E64_slot;
        int volume;
        if (sound->channel != 0) {
            volume = 0x7F;
            random = func_800D3FD8();
            func_8006DF50(sound->channel, 0x5BB, random, 0x80, volume);
            if (sound->channel != 0) {
                func_8006DF50(sound->channel, 0x5BC, 0x80, 0x80, volume);
            }
        }
    }
    if (D_800E27EC < 0x24) {
        break;
    }
    return 1;

    case 2:
    {
        int value;
        int index;
        index = D_800E11FA;
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        value = D_800E2850[index];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 4;
        D_800F3368.tpage = value;
    }
        break;
    default:
        break;
    }
    return 0;
}
