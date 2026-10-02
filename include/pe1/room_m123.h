#ifndef PE1_ROOM_M123_H
#define PE1_ROOM_M123_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"

/* Particle drawn by the room_m123 pulsing sprite callback: it scales with a
 * cosine of its frame counter, then fades into a second blended sprite. */
typedef struct RoomM123PulsingParticle {
    u16 frame;
    u16 offset;
    s16 scale;
} RoomM123PulsingParticle;

extern u8 D_80195690[];
extern u8 D_80195688[];
extern u8 D_801954BC[];
extern s16 D_80195684;

int func_80077DC4(int angle);
u16 func_80077AA4(int, int);
void func_800CF844(void *, void *, int, void *, int, int);
void func_800D1DEC(void *, void *, int, int);
void func_800783E4(void *, void *, int, int, void *);
void func_800D2B58(void *, void *, void *, int, int, int, int);
int func_80192BDC(int mode, RoomM123PulsingParticle *particle);

#endif
