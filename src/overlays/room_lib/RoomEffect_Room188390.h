#ifndef ROOM_EFFECT_ROOM188390_H
#define ROOM_EFFECT_ROOM188390_H

#include "common.h"

typedef struct RoomEffectVector {
    u16 x, y, z, pad;
} RoomEffectVector;

typedef struct RoomEffectPoolContext {
    u8 pad[8];
    void *pool;
} RoomEffectPoolContext;

typedef struct RoomEffectTimedParticle {
    u16 x, y, z;
    u8 pad06[10];
    u16 active, phase, frame;
} RoomEffectTimedParticle;

typedef struct RoomEffectTriplePoolOwner {
    u8 pad[0x238];
    u8 *map;
} RoomEffectTriplePoolOwner;

typedef struct RoomEffectTripleParticle {
    u16 x, y, z, pad;
    s16 motion[4];
    u16 kind, phase;
} RoomEffectTripleParticle;

typedef char RoomEffectVectorSizeCheck[
    sizeof(RoomEffectVector) == 0x08 ? 1 : -1];
typedef char RoomEffectPoolContextSizeCheck[
    sizeof(RoomEffectPoolContext) == 0x0C ? 1 : -1];
typedef char RoomEffectTimedParticleSizeCheck[
    sizeof(RoomEffectTimedParticle) == 0x16 ? 1 : -1];
typedef char RoomEffectTriplePoolOwnerSizeCheck[
    sizeof(RoomEffectTriplePoolOwner) == 0x23C ? 1 : -1];
typedef char RoomEffectTripleParticleSizeCheck[
    sizeof(RoomEffectTripleParticle) == 0x14 ? 1 : -1];

#endif
