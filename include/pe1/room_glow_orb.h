#ifndef PE1_ROOM_GLOW_ORB_H
#define PE1_ROOM_GLOW_ORB_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/room_fx.h"
#include "pe1/room_module.h"
#include "pe1/room_spark.h"

/* The glow orb (src/overlays/room_lib/RoomEffect_GlowOrb.c, rooms m034,
 * m174 and m383): a camera-facing glow placed by the owner's matrix,
 * turned with the script's yaw, drifting until it leaves the walkable area
 * and leaving fading flashes behind. */

/* The orb's state. The update reads the same record as a
 * RoomFallingParticleState (room_fx.h): flag2 is its frame counter, size
 * its phase, depth its intensity and offset its velocity. */
typedef struct RoomGlowOrb {
    /* 0x00 */ s8 state;
    /* 0x01 */ u8 slot;
    /* 0x02 */ u8 flag2;
    /* 0x03 */ u8 pad03;
    /* 0x04 */ s16 size;
    /* 0x06 */ u16 depth;
    /* 0x08 */ s16 x;
    /* 0x0A */ s16 y;
    /* 0x0C */ s16 z;
    /* 0x0E */ u8 pad0E[2];
    /* 0x10 */ s16 h10;
    /* 0x12 */ s16 h12;
    /* 0x14 */ s16 h14;
    /* 0x16 */ u8 pad16[2];
    /* 0x18 */ GteShortVector offset;
    /* 0x20 */ u8 pad20[4];
    /* 0x24 */ GteMatrix matrix;
} RoomGlowOrb;

/* The spawner object: it records the script's yaw and drops the orb. */
typedef struct RoomGlowOrbSpawner {
    /* 0x00 */ s16 pad00;
    /* 0x02 */ s16 yaw;
} RoomGlowOrbSpawner;

/* A flash left by the orb: fades over twenty frames. */
typedef struct RoomGlowOrbFlash {
    /* 0x00 */ u8 pad00[2];
    /* 0x02 */ s8 timer;
    /* 0x03 */ u8 pad03;
    /* 0x04 */ s16 depth;
    /* 0x06 */ u8 pad06[2];
    /* 0x08 */ s16 x;
    /* 0x0A */ s16 y;
    /* 0x0C */ s16 z;
} RoomGlowOrbFlash;

/* The owner's link record: its render node and its joint matrices. */
typedef struct RoomGlowOrbLink {
    /* 0x000 */ u32 *target;
    /* 0x004 */ u8 pad004[0x234];
    /* 0x238 */ GteMatrix *matrices;
} RoomGlowOrbLink;

typedef struct RoomGlowOrbObject {
    /* 0x00 */ u8 pad00[8];
    /* 0x08 */ RoomGlowOrbLink *link;
} RoomGlowOrbObject;

/* Per-room data, named in each room's symbol file: the module's script,
 * the owner's placement and origin matrices captured at start, the owner,
 * and the orb's and the flashes' sprite parameter blocks. The module's
 * handler lists and spawn layout are the room module lists
 * (pe1/room_module.h). */
extern int g_RoomGlowOrbScript[];
extern GteMatrix g_RoomGlowOrbAxes;
extern GteMatrix g_RoomGlowOrbOrigin;
extern RoomGlowOrbLink *g_RoomGlowOrbOwner;
extern RoomFxSpritePacket g_RoomGlowOrbSprite;
extern RoomFxSpritePacket g_RoomGlowOrbFlashSprite;

int RoomEffect_GlowOrbSetup(void);

void *memset(void *dst, int value, unsigned int size);
void func_800C2EAC(int arg0);
void func_800C2FF0(int width, int height);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C42A4(RoomFxSpritePacket *sprite, GteMatrix *matrix, int mode);
int func_800C61A8(GteShortVector *point, GteMatrix *matrix);
int FieldEng_GetStatus(RoomGlowOrbObject *object);
void **FieldEng_GetSlot(void);
u16 *func_800C2B90(void *owner, int kind, int *layout,
                   RoomModuleHandler *initList);
int *func_800C2B10(int index);
int *func_800C2B28(int index);

#endif
