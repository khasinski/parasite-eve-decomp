#ifndef PE1_ROOM_TWIN_RIBBON_H
#define PE1_ROOM_TWIN_RIBBON_H

#include "common.h"
#include "pe1/render_object.h"
#include "pe1/room_ember_burst.h"

/* Twin ribbon controller (room_m318 func_80196E4C): two mirrored pairs of
 * light ribbons hang between the actor's bones 8 and 21; a pulse is shed
 * from alternating bones every frame until the parameters stop it. */
typedef struct RoomTwinRibbon {
    s16 boneA;                    /* 0x00 */
    s16 boneB;                    /* 0x02 */
    s16 reserved04;
    u16 timer;                    /* 0x06 */
    u8 trailA[0x50];              /* 0x08 */
    u8 trailB[0x50];              /* 0x58 */
    u8 trailC[0x70];              /* 0xA8 */
    u8 trailD[0x70];              /* 0x118 */
} RoomTwinRibbon;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomTwinRibbon, trailD) == 0x118,
                  room_twin_ribbon_trail_d);

typedef struct RoomTwinRibbonParams {
    s32 stop;                     /* 0x00 */
} RoomTwinRibbonParams;

extern GteShortVector D_8018F214;
extern GteShortVector D_8018F21C;
extern GteShortVector D_8018F224;
extern GteShortVector D_8018F22C;
extern RenderColor D_8018F234;
extern RenderColor D_8018F238;
extern RenderColor D_8018F23C;
extern int func_80196C48(int mode, RoomOrbitTrailParticle *p);
extern void func_800CE8F0(void *pool, int index, void *offset, void *position);
extern void func_800D1384(void *from, void *to, int width, void *color0,
                          void *color1, int alpha, void *trail, int mode);

#endif
