#include "pe1/room_spark.h"
#include "pe1/gte.h"
#include "pe1/room_orbit_trail.h"
#ifndef ROOMEFFECT_ORBIT_SPIN_FUNC
#error "ROOMEFFECT_ORBIT_SPIN_FUNC must name the particle callback"
#endif
#ifndef ROOMEFFECT_ORBIT_SPIN_COLOR
#error "ROOMEFFECT_ORBIT_SPIN_COLOR must name the trail colour seed"
#endif
#ifndef ROOMEFFECT_ORBIT_SPIN_ORIGIN
#error "ROOMEFFECT_ORBIT_SPIN_ORIGIN must name the orbit centre"
#endif
#ifndef ROOMEFFECT_ORBIT_SPIN_TRACK
#error "ROOMEFFECT_ORBIT_SPIN_TRACK must name the trail track"
#endif

extern RenderColor ROOMEFFECT_ORBIT_SPIN_COLOR;
extern GteShortVector ROOMEFFECT_ORBIT_SPIN_ORIGIN;
extern u8 ROOMEFFECT_ORBIT_SPIN_TRACK[];
extern void func_800783E4(void *from, void *to, int weightFrom, int weightTo,
                          void *out);
extern void func_800D2B58(void *, void *, void *, void *, int, int, int);

/* Orbit trail particle variant: the flash phase draws a spinning sprite
 * with its own parameter block and the closing ring is larger and
 * slower than RoomEffect_OrbitTrailParticle's. */
int ROOMEFFECT_ORBIT_SPIN_FUNC(int mode, RoomOrbitTrailParticle *p) {
    GteShortVector offset;
    GteShortVector blend;
    GteRotation rotation;
    RenderColor color;
    int fall;
    int bounce;
    int scale;
    int weight;
    int glow;

    color = ROOMEFFECT_ORBIT_SPIN_COLOR;
    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            p->heading.x += 0x10;
            p->heading.y += 8;
            if ((s16)p->timer < 0x18) break;
            return 1;
        case 1:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 127 / 128;
            p->heading.z = p->heading.z * 127 / 128;
            fall = (u16)p->heading.y + 1;
            p->heading.y = fall;
            if (p->y >= D_800942EC.count) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            if ((s16)p->timer < 0x10) break;
            return 1;
        case 2:
            p->timer++;
            p->heading.z += 0x80;
            if ((s16)p->timer < 0xC) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0:
            scale = (func_80077CF4(((s16)p->timer << 10) / 24) + 0x1000) / 2;
            func_800CFB7C(&p->heading, (s16)(p->radius * scale / 4096), &offset);
            offset.x += ROOMEFFECT_ORBIT_SPIN_ORIGIN.x;
            offset.y += ROOMEFFECT_ORBIT_SPIN_ORIGIN.y;
            offset.z += ROOMEFFECT_ORBIT_SPIN_ORIGIN.z;
            weight = func_80077CF4(((s16)p->timer << 10) / 24);
            func_800783E4(&ROOMEFFECT_ORBIT_SPIN_ORIGIN, &offset, 0x1000 - weight,
                          weight, &blend);
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            func_800CF3AC(ROOMEFFECT_ORBIT_SPIN_TRACK, &color, (s16)p->timer);
            func_800D2B58(&blend, &offset, 0, &color, 0, 0x80, 1);
            break;
        case 1: {
            u16 clut;
            int kind;
            int palette;
            int size = func_80077CF4((s16)p->timer << 6) + 0x800;
            glow = func_80077DC4((s16)p->timer * 1204 / 16) / 128 + 0x28;
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = (s16)p->timer * 32;
            rotation.flags = 0;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x60, palette);
            func_800CEE20((GteShortVector *)p, &rotation, size, size, 0x40, clut, 1,
                          glow / 2, 0);
            break;
        }
        case 2: {
            u16 clut;
            int kind;
            int palette;
            glow = func_80077DC4(((s16)p->timer << 10) / 12) / 32;
            scale = func_80077CF4(((s16)p->timer << 10) / 12);
            /* The scaled radius shares the palette kind's temporary, which
             * is what leaves the heading pointer the first saved register. */
            kind = p->radius * scale / 4096;
            func_800CFB7C(&p->heading, (s16)kind, &offset);
            offset.x += ROOMEFFECT_ORBIT_SPIN_ORIGIN.x;
            offset.y += ROOMEFFECT_ORBIT_SPIN_ORIGIN.y;
            offset.z += ROOMEFFECT_ORBIT_SPIN_ORIGIN.z;
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            *(u32 *)&color = 0x808080;
            {
                int tpage = D_800E2850[D_800E11EA];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 0;
                D_800F3368.tpage = tpage;
            }
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800D2370(&offset, (GteRotation *)&p->heading, 1000, 320, 0x80, 0x90,
                          0x1F, 0x1F, clut, &color, &color, (s16)glow, 1);
            break;
        }
        }
        break;
    }
    return 0;
}

#undef ROOMEFFECT_ORBIT_SPIN_FUNC
#undef ROOMEFFECT_ORBIT_SPIN_COLOR
#undef ROOMEFFECT_ORBIT_SPIN_ORIGIN
#undef ROOMEFFECT_ORBIT_SPIN_TRACK
