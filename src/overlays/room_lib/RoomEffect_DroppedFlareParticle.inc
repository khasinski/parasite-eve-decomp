#include "pe1/room_flare.h"
#include "pe1/gte.h"
#ifndef ROOMEFFECT_DROPPED_FLARE_FUNC
#error "ROOMEFFECT_DROPPED_FLARE_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_DROPPED_FLARE_ROTATION
#error "ROOMEFFECT_DROPPED_FLARE_ROTATION must name the floor rotation seed"
#endif
#ifndef ROOMEFFECT_DROPPED_FLARE_COLOR
#error "ROOMEFFECT_DROPPED_FLARE_COLOR must name the colour seed"
#endif

extern GteRotation ROOMEFFECT_DROPPED_FLARE_ROTATION;
extern RoomFlareColor ROOMEFFECT_DROPPED_FLARE_COLOR;

/* Flashes the scene: marks the battle actor and rewrites the colour code of
 * the first packet in the primary effect channel. */
#define ROOMEFFECT_DROPPED_FLARE_FLASH()                                     \
    {                                                                        \
        RoomFlareChannel *channel = D_800F32D0;                              \
        RoomFlarePacket *packet;                                             \
        if (((*(RoomFlarePacket **)channel->pool)->code & 0x3F000000) ==     \
            0x01000000) {                                                    \
            D_8009D254->actor->flags |= 0x4000;                              \
            packet = *(RoomFlarePacket **)channel->pool;                     \
            packet->code = (packet->code & 0xC0FFFFFF) | 0x11000000;         \
            packet = *(RoomFlarePacket **)channel->pool;                     \
            packet->code |= 0x80000000;                                      \
        }                                                                    \
    }

/* Particle that falls under gravity, flashes the scene when it hits the
 * walkable area, scatters children every other frame, and settles on the
 * floor; the lift argument scales the sprite and the scatter spread. */
int ROOMEFFECT_DROPPED_FLARE_FUNC(int mode, RoomDroppedFlare *flare,
                                  RoomDroppedFlareSpawn *spawn) {
    GteRotation rotation = ROOMEFFECT_DROPPED_FLARE_ROTATION;
    RoomFlareColor color = ROOMEFFECT_DROPPED_FLARE_COLOR;
    GteShortVector floorPos;
    GteRotation floorSpin;
    RoomDroppedFlare *child;
    RoomFlareMatrixSlot *matrixSlot;
    int lift = spawn->lift;
    int spread;
    int scale;
    int fade;

    switch (mode) {
    case 1:
        switch (flare->state) {
        case 0:
            flare->x += flare->vx;
            flare->y += flare->vy;
            flare->z += flare->vz;
            flare->vy += 2;
            if (func_800C6B90(flare, lift * 50 / 4096)) {
                if (D_800E2368->active) {
                    ROOMEFFECT_DROPPED_FLARE_FLASH();
                }
                flare->state = 1;
                flare->timer = 0;
            }
            if (D_800E27EC & 1) {
                spread = lift * 48 / 4096;
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = flare->x;
                    child->y = flare->y;
                    child->z = flare->z;
                    child->x += rand() % spread - spread / 2;
                    child->y += rand() % spread - spread / 2;
                    child->z += rand() % spread - spread / 2;
                    child->state = 4;
                    child->swing = 0;
                    child->timer = 0;
                }
            }
            if (flare->y < D_800942EC.count) break;
            flare->state = 2;
            flare->timer = 0;
            break;
        case 1:
            flare->timer++;
            flare->y += rsin((s16)flare->timer << 9) / 512;
            if ((s16)flare->timer < 8) break;
            return 1;
        case 2:
            flare->timer++;
            if ((s16)flare->timer < 16) break;
            return 1;
        case 3:
            flare->timer++;
            flare->y -= 2;
            if ((s16)flare->timer < 24) break;
            return 1;
        case 4:
            flare->timer++;
            flare->y += flare->swing;
            flare->swing += 2;
            if (flare->y >= D_800942EC.count) {
                flare->swing = 0;
                flare->y = D_800942EC.count;
            }
            if ((s16)flare->timer < 24) break;
            return 1;
        }
        break;
    case 2:
    {
        int unit = 0x20;
        int index = D_800E11EC;
        int tpage;
        D_800F3368.parameter00 = unit;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = unit;
        D_800F3368.extent_y = unit;
        tpage = D_800E2850[index];
        D_800F3368.palette = 4;
        D_800F3368.parameter06 = 0;
        D_800F3368.tpage = tpage;
        switch (flare->state) {
        case 0:
            {
            u16 clut;
            int kind;
            int palette;
            scale = (rcos(D_800E27EC << 8) / 8 + 0x800) / 2;
            scale = lift * scale / 4096;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = GetClut(0x10, palette);
            func_800CEE20(flare, 0, scale, scale, 0x20, clut, 0xFF, 0x80,
                          &color);
            }
            {
            u16 clut;
            int kind;
            int palette;
            floorSpin.x = 0x400;
            floorSpin.y = 0;
            floorSpin.z = 0;
            floorSpin.flags = 1;
            floorPos.x = flare->x;
            floorPos.z = flare->z;
            floorPos.y = D_800942EC.count;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = GetClut(0x10, palette);
            func_800CEE20(&floorPos, &floorSpin, scale, scale, 0x20, clut, 2,
                          0x46, 0);
            }
            return 0;
        case 1:
            {
            u16 clut;
            int kind;
            int palette;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = GetClut(0, palette);
            func_800CEE20(flare, 0, lift, lift,
                          D_800F3368.parameter02 * (s16)flare->timer, clut, 1,
                          0x80, &color);
            }
            return 0;
        case 2:
            {
            u16 clut;
            int kind;
            int palette;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = GetClut(0, palette);
            func_800CEE20(flare, &rotation, lift, lift,
                          D_800F3368.parameter02 * ((s16)flare->timer / 2),
                          clut, 0, 0x80, &color);
            }
            break;
        case 3:
            {
            u16 clut;
            int kind;
            int palette;
            int unit2 = 0x10;
            int index2 = D_800E11E8;
            int tpage2;
            D_800F3368.parameter00 = unit2;
            D_800F3368.parameter02 = 1;
            D_800F3368.extent_x = unit2;
            D_800F3368.extent_y = unit2;
            tpage2 = D_800E2850[index2];
            D_800F3368.palette = 2;
            D_800F3368.parameter06 = 0;
            D_800F3368.tpage = tpage2;
            fade = rcos(((s16)flare->timer << 10) / 24) / 32;
            scale = ((s16)flare->timer << 11) / 24 + 0x1000;
            scale = lift * scale / 4096;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = GetClut(0x80, palette);
            func_800CEE20(flare, &rotation, scale, scale,
                          D_800F3368.parameter02 * 2 + 0xFD, clut, 1, fade, 0);
            }
            return 0;
        case 4:
            matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            fade = rcos(((s16)flare->timer << 10) / 24) / 32;
            func_800D1DEC(flare, &color, fade, 1);
            break;
        }
        break;
    }
    }
    return 0;
}

#undef ROOMEFFECT_DROPPED_FLARE_FLASH
#undef ROOMEFFECT_DROPPED_FLARE_FUNC
#undef ROOMEFFECT_DROPPED_FLARE_ROTATION
#undef ROOMEFFECT_DROPPED_FLARE_COLOR
