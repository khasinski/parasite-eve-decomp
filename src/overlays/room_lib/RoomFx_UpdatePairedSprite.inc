#include "common.h"

typedef struct RoomPairedSpriteControl {
    unsigned char pad0;
    unsigned char state;
} RoomPairedSpriteControl;

typedef struct RoomPairedSpriteState {
    unsigned short x;
    unsigned short y;
    unsigned short z;
    unsigned char pad6[2];
    unsigned short velocityX;
    unsigned char padA[2];
    unsigned short velocityZ;
    unsigned char padE[2];
    unsigned short scale;
    unsigned char phase;
    unsigned char pad13;
    unsigned short verticalStep;
    unsigned char active;
} RoomPairedSpriteState;

extern short g_FrameCount16;
extern unsigned char ROOMFX_UPDATE_PAIRED_SPRITE_CONFIG[];
extern unsigned char ROOMFX_UPDATE_PAIRED_SPRITE_CALLBACK[];

extern unsigned short *func_800C2B90(
    void *entity, int kind, void *config, void *callback);

void ROOMFX_UPDATE_PAIRED_SPRITE_NAME(
    void *entity, RoomPairedSpriteControl *control,
    RoomPairedSpriteState *effect) {
    void *self = entity;
    RoomPairedSpriteState *state;
    unsigned short *burst;
    char *cursor;
    unsigned short *frame;
    unsigned int i;
    int belowFloor;

    belowFloor = (short)effect->y < g_FrameCount16;
    state = effect;
    if (!belowFloor) {
        if ((short)effect->scale >= 0x15) {
            effect->scale -= 0x14;
        } else {
            control->state = 2;
        }

        if (state->active == 0) {
            state->active = 1;

            {
                unsigned short *spawn = func_800C2B90(
                    self, 3, ROOMFX_UPDATE_PAIRED_SPRITE_CONFIG,
                    ROOMFX_UPDATE_PAIRED_SPRITE_CALLBACK);
                if (spawn != 0) {
                    spawn[0] = state->x;
                    spawn[1] = g_FrameCount16;
                    spawn[2] = state->z;
                }
            }

            {
                unsigned short *spawn = func_800C2B90(
                    self, 2, ROOMFX_UPDATE_PAIRED_SPRITE_CONFIG,
                    ROOMFX_UPDATE_PAIRED_SPRITE_CALLBACK);
                if (spawn != 0) {
                    spawn[0] = state->x;
                    spawn[1] = g_FrameCount16;
                    spawn[2] = state->z;
                }
            }

            burst = func_800C2B90(
                self, 4, ROOMFX_UPDATE_PAIRED_SPRITE_CONFIG,
                ROOMFX_UPDATE_PAIRED_SPRITE_CALLBACK);
            if (burst != 0) {
                i = 0;
                frame = (unsigned short *)&g_FrameCount16;
                cursor = (char *)burst;
                do {
                    *(unsigned short *)(cursor + 0x20) = state->x;
                    *(unsigned short *)(cursor + 0x22) = *frame;
                    *(unsigned short *)(cursor + 0x24) = state->z;
                    i++;
                    cursor += 8;
                } while (i < 6);
            }
        }
    } else {
        register RoomPairedSpriteState *motion asm("$16") = state;
        PE1_COMPILER_LAUNDER(motion);
        *(unsigned short *)((char *)motion + 0x0) +=
            *(unsigned short *)((char *)motion + 0x8);
        *(unsigned short *)((char *)motion + 0x4) +=
            *(unsigned short *)((char *)motion + 0xC);
        *(unsigned short *)((char *)motion + 0x2) +=
            (short)*(unsigned short *)((char *)motion + 0x14) >> 8;
        *(unsigned short *)((char *)motion + 0x10) += 0x28;
        *(unsigned short *)((char *)motion + 0x14) += 0x64;
    }
}
