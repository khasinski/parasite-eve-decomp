#include "pe1/room_orbit_spin_burst.h"
#include "pe1/gte.h"
#ifndef ROOMEFFECT_ORBIT_SPIN_BURST_FUNC
#error "ROOMEFFECT_ORBIT_SPIN_BURST_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_ORBIT_SPIN_BURST_CALLBACK
#error "ROOMEFFECT_ORBIT_SPIN_BURST_CALLBACK must name the orbit spin particle"
#endif
#ifndef ROOMEFFECT_ORBIT_SPIN_BURST_COLOR0
#error "ROOMEFFECT_ORBIT_SPIN_BURST_COLOR0 must name the flash colour seed"
#endif
#ifndef ROOMEFFECT_ORBIT_SPIN_BURST_COLOR1
#error "ROOMEFFECT_ORBIT_SPIN_BURST_COLOR1 must name the ring colour seed"
#endif
#ifndef ROOMEFFECT_ORBIT_SPIN_BURST_ORIGIN
#error "ROOMEFFECT_ORBIT_SPIN_BURST_ORIGIN must name the shared orbit centre"
#endif

extern RenderColor ROOMEFFECT_ORBIT_SPIN_BURST_COLOR0;
extern RenderColor ROOMEFFECT_ORBIT_SPIN_BURST_COLOR1;
extern GteShortVector ROOMEFFECT_ORBIT_SPIN_BURST_ORIGIN;
extern int ROOMEFFECT_ORBIT_SPIN_BURST_CALLBACK(int mode, RoomOrbitTrailParticle *p);

/* Controller that bursts orbit spin particles from the actor during the
 * first four frames (falling sparks, sweeping trails and drifting rings),
 * flashes a glow, a halo, two tilted rings and three glow sprites while it
 * fades, and publishes the actor position as the orbit centre. */
int ROOMEFFECT_ORBIT_SPIN_BURST_FUNC(int mode, RoomOrbitTrailBurst *burst) {
    RoomOrbitSpinBurstTilt tilt;
    RenderColor color = ROOMEFFECT_ORBIT_SPIN_BURST_COLOR0;
    RenderColor ringColor = ROOMEFFECT_ORBIT_SPIN_BURST_COLOR1;
    RoomOrbitTrailParticle *child;
    int i;

    switch (mode) {
    case 0:
        burst->counterB = 0;
        burst->counterA = 0;
        func_800D3F64(0x5DF, func_800D3FD8());
        func_800D3F64(0x5E0, 0x80);
        func_800CE870(D_8009D254, 0, (s16 *)burst);
        return func_800CE560(D_800F33E0->pool, 0x14, 0x3C,
                             ROOMEFFECT_ORBIT_SPIN_BURST_CALLBACK);
    case 1:
        if (D_800E27EC < 4) {
            for (i = 0; i < 4; i++) {
                child = (RoomOrbitTrailParticle *)func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->heading.x = func_80071A54() % 32 - 0x10;
                    child->heading.y = func_80071A54() % 32 - 0x10;
                    child->heading.z = func_80071A54() % 32 - 0x10;
                    child->state = 1;
                    child->timer = 0;
                }
            }
            for (i = 0; i < 7; i++) {
                child = (RoomOrbitTrailParticle *)func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->heading.x = func_80071A54();
                    child->heading.y = func_80071A54();
                    child->heading.z = func_80071A54();
                    child->radius = (func_80071A54() & 0x3FF) + 0x5DC;
                    child->state = 0;
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
                    child->radius = (func_80071A54() & 0xFF) + 0x100;
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
            int glow = func_80077DC4((D_800E27EC << 10) / 20) / 32;
            int scale;
            {
                u16 clut;
                int kind;
                int palette;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0x90, palette);
                func_800CEE20((GteShortVector *)burst, 0, 0x2000, 0x2000, 0x98,
                              clut, 1, glow, 0);
            }
            func_800D004C((GteShortVector *)burst, 0x320, 0x320, 12, 0, 0x1000,
                          0x1000, &color, 0, glow, 1);
            scale = func_80077CF4((D_800E27EC << 10) / 20) / 2 + 0x1000;
            func_800D0728((GteShortVector *)burst, 0x226, 0x320, 0x14, 0, scale,
                          scale, 0, &color, glow / 2, 1);
            func_800CE9D4((struct RoomFxTransformOwner *)D_800F32D0->pool, 0,
                          &tilt.vector);
            scale = func_80077CF4((D_800E27EC << 10) / 20);
            tilt.rotation.flags = 1;
            tilt.rotation.x += 0x200;
            tilt.rotation.y += 0x400;
            func_800D0728((GteShortVector *)burst, 0x44C, 0x578, 0x14,
                          &tilt.rotation, scale, scale, 0, &ringColor, glow, 1);
            tilt.rotation.x -= 0x400;
            func_800D0728((GteShortVector *)burst, 0x44C, 0x578, 0x14,
                          &tilt.rotation, scale, scale, 0, &ringColor, glow, 1);
            glow = func_80077DC4((D_800E27EC << 10) / 20) / 32;
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            {
                u16 clut;
                int kind;
                int palette;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0xA0, palette);
                func_800D3BC8((GteShortVector *)burst, 0x1000, 0x1000, 12, clut,
                              1, glow, 0, 0xC00);
            }
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            {
                u16 clut;
                int kind;
                int palette;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0xB0, palette);
                func_800D3BC8((GteShortVector *)burst, 0x1000, 0x1000, 0x9A, clut,
                              1, glow, 0, 0xA00);
            }
            {
                u16 clut;
                int kind;
                int palette;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0xC0, palette);
                func_800D3BC8((GteShortVector *)burst, 0x1000, 0x1000, 0x9C, clut,
                              1, glow, 0, 0x600);
            }
        }
        ROOMEFFECT_ORBIT_SPIN_BURST_ORIGIN.x = burst->x;
        ROOMEFFECT_ORBIT_SPIN_BURST_ORIGIN.y = burst->y;
        ROOMEFFECT_ORBIT_SPIN_BURST_ORIGIN.z = burst->z;
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
