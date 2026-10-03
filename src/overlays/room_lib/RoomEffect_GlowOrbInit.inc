#include "pe1/gte.h"
#include "pe1/room_glow_orb.h"
#include "pe1/room_spark.h"
#ifndef ROOMEFFECT_GLOW_ORB_INIT_FUNC
#error "ROOMEFFECT_GLOW_ORB_INIT_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_GLOW_ORB_INIT_SPIN
#error "ROOMEFFECT_GLOW_ORB_INIT_SPIN must name the local spin rotation"
#endif
#ifndef ROOMEFFECT_GLOW_ORB_INIT_AXES
#error "ROOMEFFECT_GLOW_ORB_INIT_AXES must name the placement matrix"
#endif
#ifndef ROOMEFFECT_GLOW_ORB_INIT_X
#error "ROOMEFFECT_GLOW_ORB_INIT_X must name the start x"
#endif
#ifndef ROOMEFFECT_GLOW_ORB_INIT_Z
#error "ROOMEFFECT_GLOW_ORB_INIT_Z must name the start z"
#endif

extern GteShortVector ROOMEFFECT_GLOW_ORB_INIT_SPIN;
extern GteMatrix ROOMEFFECT_GLOW_ORB_INIT_AXES;
extern RoomGlowOrbWord ROOMEFFECT_GLOW_ORB_INIT_X;
extern RoomGlowOrbWord ROOMEFFECT_GLOW_ORB_INIT_Z;

void ROOMEFFECT_GLOW_ORB_INIT_FUNC(void *object, void *slot, RoomGlowOrb *orb) {
    GteShortVector tilt;
    GteShortVector spin = ROOMEFFECT_GLOW_ORB_INIT_SPIN;
    GteMatrix local;
    GteShortVector turn;
    GteMatrix base;
    int *yaw;

    turn.x = 0;
    yaw = func_800C2B10(4);
    turn.y = *yaw + *func_800C2B28(2);
    turn.z = 0;
    RotMatrixYXZ(&turn, &base);
    tilt.x = 0;
    tilt.y = 0;
    tilt.z = -*func_800C2B28(0);
    ApplyMatrixSV(&ROOMEFFECT_GLOW_ORB_INIT_AXES, &tilt, &orb->offset);
    ApplyMatrixSV(&base, &orb->offset, &orb->offset);
    orb->matrix = ROOMEFFECT_GLOW_ORB_INIT_AXES;
    orb->x = ROOMEFFECT_GLOW_ORB_INIT_X.value;
    orb->y = D_800942EC.count - 0x100;
    orb->z = ROOMEFFECT_GLOW_ORB_INIT_Z.value;
    orb->size = 0x80;
    orb->h10 = 0;
    orb->h12 = 0;
    orb->h14 = 0;
    orb->depth = 0;
    orb->flag2 = 0;
    orb->state = 1;
    orb->slot = *func_800C2B10(3);
    RotMatrixYXZ(&spin, &local);
    gte_CompMatrix(&orb->matrix, &local, &orb->matrix);
    gte_CompMatrix(&base, &orb->matrix, &orb->matrix);
}
