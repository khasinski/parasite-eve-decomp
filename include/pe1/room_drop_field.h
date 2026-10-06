#ifndef PE1_ROOM_DROP_FIELD_H
#define PE1_ROOM_DROP_FIELD_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/room_fx.h"
#include "pe1/field_ring_geometry.h"
#include "pe1/field_collision.h"
#include "pe1/room_spark.h"
#include "pe1/field_sprite_state.h"
#include "pe1/room_drop_field_class.h"

/* The drop field effect set linked by room_m174, room_m348 and room_m383
 * (src/overlays/room_lib/RoomFx_DropFieldEffects.c): a controller and
 * twelve init/draw/update triples, among them the drop field, a beam pair,
 * a ring pair, staged motion and eight particles. */

/* The room entity and its link as the controller init reads them. */
typedef struct RoomDropFieldLink {
    u8 pad0[0x238];
    void *view;                   /* 0x238: view matrices */
} RoomDropFieldLink;

typedef struct RoomDropFieldEntity {
    u8 pad0[0x8];
    RoomDropFieldLink *link;      /* 0x08 */
} RoomDropFieldEntity;

typedef struct RoomDropFieldMatrixWords {
    int w0;
    int w1;
    int w2;
    int w3;
    int w4;
    int w5;
    int w6;
    int w7;
} RoomDropFieldMatrixWords;

typedef struct RoomDropFieldState {
    RoomDropFieldLink *link;
    RoomDropFieldMatrixWords matrix0;
    RoomDropFieldMatrixWords matrix1;
    void *asset;
} RoomDropFieldState;

typedef struct RoomDropFieldQuad {
    s32 x;
    s32 y;
    s32 z;
    s32 w;
} RoomDropFieldQuad;

typedef struct RoomDropFieldVec {
    s16 x, y, z, pad;
} RoomDropFieldVec;

/* State of the sixteen drops. */
typedef struct RoomDropFieldDrops {
    u8 flag[16];                  /* 0x00 */
    u8 pad10[0x10];
    s16 height[16];               /* 0x20 */
    s16 scale[16];                /* 0x40 */
    RoomDropFieldVec pos[16];     /* 0x60 */
    RoomDropFieldVec vel[16];     /* 0xE0 */
    RoomDropFieldQuad anchor;     /* 0x160 */
    RoomDropFieldQuad facing;     /* 0x170 */
    s16 h180, h182, h184;
} RoomDropFieldDrops;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomDropFieldDrops, h180) == 0x180,
                  room_drop_field_drops_h180);

/* Parameter block reset by the model sprite init. */
typedef struct RoomDropFieldDecal {
    char pad0[0x8];
    short h8;
    short hA;
    short hC;
    char padE[0x2];
    short h10;
    short h12;
} RoomDropFieldDecal;

/* Stack frame of the two rotated sprite draws. */
typedef struct RoomDropFieldSpinFrame {
    GteVector sp10;
    s32 pad20;
    s32 sp24[3];
    GteVector sp30;
    GteVector sp40;
} RoomDropFieldSpinFrame;

PE1_STATIC_ASSERT(sizeof(RoomDropFieldSpinFrame) == 0x40,
                  room_drop_field_spin_frame_size);

/* Per-room data: seven sprite records, four ring records (the beam pair
 * draws the last two), the ring and beam images and the particle animation
 * table. The spawn tables are declared with the module class
 * (pe1/room_drop_field_class.h). */
extern RoomFxSpritePacket g_RoomDropFieldSpinSprite;
extern RoomFxSpritePacket g_RoomDropFieldModelSprite;
/* The model the controller loads; it follows the model sprite record. */
extern void *g_RoomDropFieldModel;
extern RoomFxSpritePacket g_RoomDropFieldDoubleSprite;
extern RoomFxSpritePacket g_RoomDropFieldParticleSprite;
extern RoomFxSpritePacket g_RoomDropFieldQuadSprite;
extern RoomFxEmitterParams g_RoomDropFieldRings[4];
extern RoomFxSpritePacket g_RoomDropFieldDropSprite;
extern RoomFxSpritePacket g_RoomDropFieldMotionSprite;
extern char g_RoomDropFieldRingImages[];
extern unsigned char g_RoomDropFieldBeamImages[];
extern u8 g_RoomDropFieldParticleFrames[];

void *func_8006DC18(int type);
int rand(void);
void func_80071A44(void *dst, s32 value, s32 size);
s32 *func_800C2B10(s32 index);
int func_800C2B68(void);
void func_800C4E50(void *params);
void func_800C6800(s32 arg0, s32 arg1, void *arg2);
void func_800C6C18(int arg0);
void func_800C6EE8(int arg0);

#endif
