#ifndef PE1_ROOM_FX_H
#define PE1_ROOM_FX_H

/* Common room-overlay sprite effect data used by m137-family rooms. */
typedef struct RoomSpriteMatrix {
    short m[3][3];
    short pad;
    int t[3];
} RoomSpriteMatrix;

typedef struct RoomFxSeed8 {
    unsigned char bytes[8];
} RoomFxSeed8;

typedef struct RoomFxVec4 {
    int x;
    int y;
    int z;
    int w;
} RoomFxVec4;

typedef struct RoomSpriteFxParams {
    short x;
    short y;
    short z;
    unsigned char pad6[0xA];
    short scale;
    unsigned short depth;
    unsigned char pad14;
    unsigned char alpha;
} RoomSpriteFxParams;

/* Per-particle state for the room sparkle/drift effect. */
typedef struct RoomFxDriftParticle {
    short x;
    short y;
    short z;
    short pad6;
    int radius;
} RoomFxDriftParticle;

#endif
