#ifndef ROOMLIB_INIT_PARTICLE_RENDER_PAIR_NAME
#error "ROOMLIB_INIT_PARTICLE_RENDER_PAIR_NAME must name the room entry point"
#endif

#include "common.h"
#include "pe1/room_fx.h"

typedef struct RoomParticleRenderRecord {
    void *packet;
    s16 field04;
    s16 field06;
    s16 field08;
    s16 field0A;
    s16 field0C;
    u8 pad0E[2];
    u8 field10;
    u8 field11;
    u8 field12;
    u8 pad13;
    RoomSpriteMatrix matrix;
} RoomParticleRenderRecord;

typedef struct RoomParticleRenderPairState {
    u8 pad00[0x78];
    RoomParticleRenderRecord first;
    RoomParticleRenderRecord second;
} RoomParticleRenderPairState;

extern char D_801940C8;
extern char D_80194370;
extern s16 D_800942EC;

void **func_800C2B50(void);
void func_800C5538(RoomParticleRenderRecord *record);
void func_800C66C8(void *owner, int message, RoomSpriteMatrix *matrix);

void ROOMLIB_INIT_PARTICLE_RENDER_PAIR_NAME(
    void *owner, void *unused, RoomParticleRenderPairState *state) {
    RoomSpriteMatrix secondMatrix;
    RoomSpriteMatrix firstMatrix;
    register void *callOwner asm("$23");
    register RoomParticleRenderPairState *workState asm("$16");
    void **root;

    callOwner = owner;
    workState = state;
    root = func_800C2B50();
    firstMatrix = *(RoomSpriteMatrix *)(*(char **)((char *)*root + 0x238));
    secondMatrix = *(RoomSpriteMatrix *)(*(char **)((char *)*root + 0x238));

    firstMatrix.t[0] = *(s32 *)(*(char **)((char *)*root + 0x238) + 0x134);
    firstMatrix.t[1] = D_800942EC;
    firstMatrix.t[2] = *(s32 *)(*(char **)((char *)*root + 0x238) + 0x13C);
    secondMatrix.t[0] = *(s32 *)(*(char **)((char *)*root + 0x238) + 0x1B4);
    secondMatrix.t[1] = D_800942EC;
    secondMatrix.t[2] = *(s32 *)(*(char **)((char *)*root + 0x238) + 0x1BC);

    workState->second.field04 = 10;
    workState->second.packet = &D_80194370;
    workState->second.field06 = 0;
    workState->second.field08 = 400;
    workState->second.field0A = 300;
    workState->second.field0C = 2;
    workState->second.matrix = secondMatrix;
    workState->second.field10 = 32;
    workState->second.field11 = 1;
    workState->second.field12 = 1;
    func_800C5538(&workState->second);

    workState->first.field04 = 10;
    workState->first.packet = &D_801940C8;
    workState->first.field06 = 0;
    workState->first.field08 = 400;
    workState->first.field0A = 300;
    workState->first.field0C = 2;
    workState->first.matrix = firstMatrix;
    workState->first.field10 = 32;
    workState->first.field11 = 1;
    workState->first.field12 = 1;
    func_800C5538(&workState->first);

    func_800C66C8(callOwner, 0x576, &firstMatrix);
}

#undef ROOMLIB_INIT_PARTICLE_RENDER_PAIR_NAME
