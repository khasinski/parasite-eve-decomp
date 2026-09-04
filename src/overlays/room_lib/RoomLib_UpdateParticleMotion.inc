#include "common.h"

extern int func_80071A54(void);

typedef struct RoomRandomMotionView {
    char pad00[0x160];
    s16 x;
    s16 z;
} RoomRandomMotionView;

typedef struct RoomParticleMotionView {
    char pad00[0x40];
    u16 x0;
    u16 x1;
    u16 x2;
    u16 pad46;
    u16 x4;
    u16 x5;
    u16 x6;
    char pad4E[0x72];
    u16 v0;
    u16 v1;
    u16 v2;
    u16 padC6;
    u16 v4;
    u16 v5;
    u16 v6;
} RoomParticleMotionView;

void ROOMLIB_UPDATE_PARTICLE_MOTION_NAME(int arg0, char *control, char *state) {
    int i;
    register RoomRandomMotionView *randomCursor asm("$16");
    register RoomParticleMotionView *motionCursor asm("$6");
    char *controlReg = control;
    char *stateReg = state;
    int loaded;
    int value;
    volatile char scratch[0x18];

    PE1_COMPILER_LAUNDER_MEM(controlReg);

    i = 0;
    if (*(s16 *)(stateReg + 0x182) != 0) {
        randomCursor = (RoomRandomMotionView *)stateReg;
        do {
            randomCursor->x = func_80071A54() % 8;
            randomCursor->z = func_80071A54() % 8;
            loaded = *(s16 *)(stateReg + 0x182);
            PE1_COMPILER_MEMORY_BARRIER();
            i++;
            randomCursor =
                (RoomRandomMotionView *)((char *)randomCursor + 4);
        } while ((u32)i < loaded);
    }

    if (*(s16 *)(controlReg + 2) >= 31) {
        loaded = *(s16 *)(stateReg + 0x180);
        value = loaded;
        PE1_COMPILER_LAUNDER(value);
        if (loaded >= 5) {
            *(s16 *)(stateReg + 0x180) = value - 4;
        } else {
            *(s16 *)(stateReg + 0x180) = 0;
        }

        i = 0;
        if (*(s16 *)(stateReg + 0x182) != 0) {
            motionCursor = (RoomParticleMotionView *)stateReg;
            do {
                i++;
                motionCursor->x0 += motionCursor->v0;
                motionCursor->x1 += motionCursor->v1;
                motionCursor->x2 += motionCursor->v2;
                motionCursor->x4 += motionCursor->v4;
                motionCursor->x5 += motionCursor->v5;
                motionCursor->x6 += motionCursor->v6;
                motionCursor =
                    (RoomParticleMotionView *)((char *)motionCursor + 0x10);
            } while ((u32)i < *(s16 *)(stateReg + 0x182));
        }
    }

    if ((u32)(*(u16 *)(controlReg + 2) - 1) < 31) {
        *(u16 *)(stateReg + 0x180) += 4;
    }
    if (*(s16 *)(controlReg + 2) == 65) {
        controlReg[1] = 2;
    }
}
