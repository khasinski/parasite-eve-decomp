#include "pe1/room_spark.h"
#include "pe1/gte.h"
#include "pe1/room_orbit_trail.h"
#ifndef ROOMEFFECT_ORBIT_BURST_FUNC
#error "ROOMEFFECT_ORBIT_BURST_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_ORBIT_BURST_CALLBACK
#error "ROOMEFFECT_ORBIT_BURST_CALLBACK must name the orbit trail particle"
#endif
#ifndef ROOMEFFECT_ORBIT_BURST_COLOR
#error "ROOMEFFECT_ORBIT_BURST_COLOR must name the flash colour seed"
#endif
#ifndef ROOMEFFECT_ORBIT_BURST_ORIGIN
#error "ROOMEFFECT_ORBIT_BURST_ORIGIN must name the shared orbit centre"
#endif

extern s32 D_800E27EC;
extern void *D_8009D254;
extern RenderColor ROOMEFFECT_ORBIT_BURST_COLOR;
extern GteShortVector ROOMEFFECT_ORBIT_BURST_ORIGIN;
extern int ROOMEFFECT_ORBIT_BURST_CALLBACK(int mode, RoomOrbitTrailParticle *p);

/* Controller that bursts orbit trail particles from the actor during the
 * first four frames (falling sparks, sweeping trails and drifting rings),
 * flashes a glow, ring and halo while it fades, and publishes the actor
 * position as the orbit centre the trails sweep out from. */
int ROOMEFFECT_ORBIT_BURST_FUNC(int mode, RoomOrbitTrailBurst *burst) {
    RenderColor color = ROOMEFFECT_ORBIT_BURST_COLOR;
    RoomOrbitTrailParticle *child;
    int i;

    switch (mode) {
    case 0:
        burst->counterB = 0;
        burst->counterA = 0;
        func_800CE870(D_8009D254, 0, (s16 *)burst);
        func_800D3F64(0x596, func_800D3FD8());
        return func_800CE560(D_800F33E0->pool, 0x14, 0x28,
                             ROOMEFFECT_ORBIT_BURST_CALLBACK);
    case 1:
        if (D_800E27EC < 4) {
            for (i = 0; i < 3; i++) {
                child = (RoomOrbitTrailParticle *)func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->heading.x = func_80071A54() % 70 - 0x23;
                    child->heading.y = func_80071A54() % 70 - 0x23;
                    child->heading.z = func_80071A54() % 70 - 0x23;
                    child->state = 1;
                    child->timer = 0;
                }
            }
            for (i = 0; i < 5; i++) {
                child = (RoomOrbitTrailParticle *)func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->heading.x = func_80071A54();
                    child->heading.y = func_80071A54();
                    child->heading.z = func_80071A54();
                    child->radius = func_80071A54() % 800 + 0x200;
                    child->state = 0;
                    child->timer = 0;
                }
            }
            for (i = 0; i < 4; i++) {
                child = (RoomOrbitTrailParticle *)func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->heading.x = func_80071A54();
                    child->heading.y = func_80071A54();
                    child->heading.z = func_80071A54();
                    child->radius = (func_80071A54() & 0x1FF) + 0x100;
                    child->state = 2;
                    child->timer = 0;
                }
            }
        }
        if (D_800E27EC < 0x1C) break;
        return 1;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0x20;
            D_800F3368.tpage = tpage;
        }
        if (D_800E27EC < 0x15) {
            u16 clut;
            int kind;
            int palette;
            int glow = func_80077DC4((D_800E27EC << 10) / 20) / 32;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800CEE20((GteShortVector *)burst, 0, 0x2000, 0x2000,
                          (s16)D_800F3368.parameter02 * (D_800E27EC / 3) + 0x20,
                          clut, 1, glow, 0);
            func_800D004C((GteShortVector *)burst, 500, 500, 12, 0, 0x1000, 0x1000,
                          &color, 0, glow, 1);
            {
                int scale = func_80077CF4((D_800E27EC << 10) / 20) / 4 + 0x1000;
                func_800D0728((GteShortVector *)burst, 0x15E, 0x190, 0x18, 0, scale,
                              scale, 0, &color, glow / 2, 1);
            }
        }
        ROOMEFFECT_ORBIT_BURST_ORIGIN.x = burst->x;
        ROOMEFFECT_ORBIT_BURST_ORIGIN.y = burst->y;
        ROOMEFFECT_ORBIT_BURST_ORIGIN.z = burst->z;
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 8;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}

#undef ROOMEFFECT_ORBIT_BURST_FUNC
#undef ROOMEFFECT_ORBIT_BURST_CALLBACK
#undef ROOMEFFECT_ORBIT_BURST_COLOR
#undef ROOMEFFECT_ORBIT_BURST_ORIGIN
