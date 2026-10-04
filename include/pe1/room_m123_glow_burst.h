#ifndef PE1_ROOM_M123_GLOW_BURST_H
#define PE1_ROOM_M123_GLOW_BURST_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"
#include "pe1/room_m123_burst_particle.h"

/* room_m123 glow burst (func_80192F0C): spawns pulsing particles at frame
 * 1, then layers four glow sprites, two flash rings and a swept flare on a
 * boss joint while publishing the joint for the pulsing particles. */
typedef struct RoomM123GlowBurst {
    s16 timer;
} RoomM123GlowBurst;

typedef struct RoomM123BurstTemplate {
    u8 bytes[8];
} RoomM123BurstTemplate;

extern RoomM123BurstTemplate D_8018F1E0;
extern s16 D_80195684;
/* Joint angles published for the pulsing particles: written by the joint
 * transform as a vector, read back by the flare sweep as a rotation. */
typedef union RoomM123JointAngles {
    GteShortVector vector;
    GteRotation rotation;
} RoomM123JointAngles;

extern RoomM123JointAngles D_80195688;
extern GteShortVector D_80195690;
extern u8 D_801954BC[];
extern u8 D_801954E4[];
extern u16 D_800E11E8;

int func_80077DC4(int angle);
u16 func_80077AA4(int, int);
int func_800CE560(void *pool, int size, int count, void *callback);
RoomM123BurstParticle *func_800CE610(void *pool);
void func_800CE8F0(void *pool, int index, void *rotation, void *position);
int func_80071A54(void);
int func_800D3FD8(void);
int func_800D3F64(int sound, int handle);
void func_800D3BC8(GteShortVector *position, int scale_x, int scale_y, int texture,
                   int clut, int page, int intensity, int unused, int angle);
int func_80192BDC(int mode, RoomM123BurstParticle *particle);

#endif
