#ifndef PE1_ROOM_M123_H
#define PE1_ROOM_M123_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"
#include "pe1/room_m123_joint_triangle.h"
#include "pe1/room_m123_burst_particle.h"

/* Particle drawn by the room_m123 pulsing sprite callback: it scales with a
 * cosine of its frame counter, then fades into a second blended sprite. */
typedef RoomM123BurstParticle RoomM123PulsingParticle;

extern u8 D_801954BC[];
extern s16 D_80195684;

int func_80077DC4(int angle);
u16 func_80077AA4(int, int);
void func_800CF844(void *, void *, int, void *, int, int);
void func_800D1DEC(void *, void *, int, int);
void func_800783E4(void *, void *, int, int, void *);
void func_800D2B58(void *, void *, void *, void *, int, int, int);
int func_80192BDC(int mode, RoomM123PulsingParticle *particle);

/* Joint triangle: three boss joint points joined by lines that pull toward a
 * fourth joint, then a spark that runs around the triangle edges. */
extern GteShortVector D_8018F1CC; /* joint offsets */
extern GteShortVector D_8018F1D4;
extern RenderColor D_8018F1DC;

#endif
