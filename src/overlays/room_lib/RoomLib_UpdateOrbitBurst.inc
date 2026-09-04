#include "common.h"

typedef struct RoomOrbitBurstPosition {
    unsigned short x;
    unsigned short y;
    unsigned short z;
    unsigned short pad6;
} RoomOrbitBurstPosition;

typedef struct RoomOrbitBurstMotion {
    unsigned short xStep;
    unsigned short yStep;
    unsigned short zStep;
    unsigned short pad6;
} RoomOrbitBurstMotion;

typedef struct RoomOrbitBurstState {
    RoomOrbitBurstPosition positions[8];
    RoomOrbitBurstMotion motion[8];
    unsigned short phase[8][4];
    unsigned char active[8];
    unsigned char padC8[0xA];
    short timer;
    unsigned char activeCount;
} RoomOrbitBurstState;

extern short D_800942EC;

int func_80071A54(void);

void ROOMLIB_UPDATE_ORBIT_BURST_NAME(
    void *unused, unsigned char *control, RoomOrbitBurstState *state) {
    unsigned char *particle;
    unsigned int i;

    if (state->timer >= 9) {
        state->timer -= 8;
    }

    i = 0;
    particle = (unsigned char *)state;
    while (i < 8) {
        if (state->active[i] == 1) {
            int xStep =
                (signed char)(*(unsigned short *)(particle + 0x40) >> 8);
            int yStep =
                (signed char)(*(unsigned short *)(particle + 0x42) >> 8);
            int zStep =
                (signed char)(*(unsigned short *)(particle + 0x44) >> 8);

            *(unsigned short *)(particle + 0) += xStep;
            *(unsigned short *)(particle + 2) += yStep;
            *(unsigned short *)(particle + 4) += zStep;
            *(unsigned short *)(particle + 0x42) += 0x190;
            *(unsigned short *)(particle + 0x84) +=
                func_80071A54() % 0x200;
            if (*(short *)(particle + 2) > D_800942EC) {
                state->activeCount--;
                state->active[i] = 0;
            }
        }
        i++;
        particle += 8;
    }

    if (state->activeCount == 0) {
        control[1] = 2;
    }
}
