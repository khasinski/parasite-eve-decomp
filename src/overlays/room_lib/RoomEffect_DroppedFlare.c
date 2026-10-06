/* MASPSX_FLAGS: --expand-div */
/*
 * Dropped flares: the particle, the emitter that drops them and the setter
 * of the room's two flare arguments. The three functions follow each other
 * in this order in thirteen hospital rooms and in scenes e09 to e13, and
 * the unit's rodata (the particle's two seed initialisers, its two jump
 * tables, the emitter's rectangle) is one contiguous block in all of them.
 */
#include "common.h"
#include "pe1/room_flare.h"
#include "pe1/gte.h"

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
int RoomEffect_DroppedFlareParticle(int mode, RoomDroppedFlare *flare,
                                    RoomDroppedFlareSpawn *spawn) {
    GteRotation rotation = {0x400, 0, 0, 1};
    RoomFlareColor color = {0xC8, 0x08, 0x00, 0x00};
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
            flare->x += flare->velocity.x;
            flare->y += flare->velocity.y;
            flare->z += flare->velocity.z;
            flare->velocity.y += 2;
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
            if (flare->y < g_RoomFloorY->y) break;
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
            if (flare->y >= g_RoomFloorY->y) {
                flare->swing = 0;
                flare->y = g_RoomFloorY->y;
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
            floorPos.y = g_RoomFloorY->y;
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
            int index2 = D_800E11E8.index;
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

/* Drops the flares: mode 0 aims the emitter (at the scene anchor, or at the
 * player) and spawns the particle, mode 1 sheds a falling flare and a
 * rising spark on frame 1, mode 2 resets the sprite parameters. */
int RoomEffect_DroppedFlareEmitter(int mode, RoomDroppedFlareEmitter *emitter,
                                   s16 *speed) {
    RoomFlareRect rect = {{0x00, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0x00, 0x00}};

    switch (mode) {
    case 0:
        func_800CE8F0(D_800F32D0->pool, 0x13, &rect, emitter);
        switch (D_800E2368->phase) {
        case 0:
            func_800CE9D4(D_800F32D0->pool, 0x13, &emitter->angles);
            break;
        case 1:
            {
                s16 target[4];
                func_800CE870((char *)D_8009D254, 0, target);
                func_800CFAA8(emitter, target, &emitter->angles);
                emitter->angles.x = 0x180;
            }
            break;
        }
        if (D_800E2368->active != 0) {
            RoomFlareNode **node = (RoomFlareNode **)D_800F32D0->pool;
            if (node != 0 && *node != 0) {
                u8 *state = (*node)->state;
                if (*state == 1) {
                    *state = 2;
                }
            }
        }
        return func_800CE560(D_800F33E0->pool, 0x14, 0x18,
                             RoomEffect_DroppedFlareParticle);
    case 1:
        if (D_800E27EC == mode) {
            RoomDroppedFlare *flare = func_800CE610(D_800F33E0->pool);
            if (flare != 0) {
                flare->x = emitter->x;
                flare->y = emitter->y;
                flare->z = emitter->z;
                func_800CFB7C(&emitter->angles, *speed, &flare->velocity);
                flare->state = 0;
                flare->timer = 0;
            }
            flare = func_800CE610(D_800F33E0->pool);
            if (flare != 0) {
                flare->x = emitter->x;
                flare->y = emitter->y;
                flare->z = emitter->z;
                flare->state = 3;
                flare->timer = 0;
            }
            func_800D3F64(0x586, func_800D3FD8());
        }
        if (D_800E27EC >= 2) {
            return 2;
        }
        break;
    case 2:
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        break;
    }
    return 0;
}

/* Stores the room's two flare arguments and returns the first. */
int *RoomEffect_DroppedFlareSetArgs(int unused, int first, int second) {
    int *slot = &RoomLib_PairA;
    *slot = first;
    RoomLib_PairB = second;
    return slot;
}
