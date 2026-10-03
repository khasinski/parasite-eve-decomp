#include "pe1/room_flare.h"
#ifndef ROOMEFFECT_ORBIT_FLARE_FUNC
#error "ROOMEFFECT_ORBIT_FLARE_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_ORBIT_FLARE_ROTATION
#error "ROOMEFFECT_ORBIT_FLARE_ROTATION must name the attachment rotation seed"
#endif
#ifndef ROOMEFFECT_ORBIT_FLARE_SPIN
#error "ROOMEFFECT_ORBIT_FLARE_SPIN must name the spin seed"
#endif
#ifndef ROOMEFFECT_ORBIT_FLARE_CALLBACK
#error "ROOMEFFECT_ORBIT_FLARE_CALLBACK must name the drift particle callback"
#endif

extern GteRotation ROOMEFFECT_ORBIT_FLARE_ROTATION;
extern GteRotation ROOMEFFECT_ORBIT_FLARE_SPIN;
extern int ROOMEFFECT_ORBIT_FLARE_CALLBACK(int mode, RoomFxDriftParticle *rec);

/* Flashes the scene: marks the battle actor and rewrites the colour code of
 * the first packet in the primary effect channel. */
#define ROOMEFFECT_ORBIT_FLARE_FLASH()                                       \
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

/* Controller that rises from the scene anchor for fifty-two frames while
 * shedding drift particles, then orbits the anchor at the spawn radius until
 * it leaves the walkable area or touches the floor, flashing the scene. */
int ROOMEFFECT_ORBIT_FLARE_FUNC(int mode, RoomOrbitFlareState *state,
                                RoomOrbitFlareSpawn *spawn) {
    GteRotation rotation = ROOMEFFECT_ORBIT_FLARE_ROTATION;
    GteRotation spin = ROOMEFFECT_ORBIT_FLARE_SPIN;
    GteShortVector floorPos;
    GteRotation floorSpin;
    GteShortVector floorPos2;
    GteRotation floorSpin2;
    RoomFxDriftParticle *child;
    char *pool;
    int height;
    int scale;
    int fade;

    switch (mode) {
    case 0:
        state->x = ((RoomFlareAnchor *)D_800F32D0->pool)->x;
        state->y = ((RoomFlareAnchor *)D_800F32D0->pool)->y;
        state->z = ((RoomFlareAnchor *)D_800F32D0->pool)->z;
        state->state = 0;
        state->frame = 0;
        state->soundHandle = -1;
        switch (D_800E2368->phase) {
        case 0:
            state->angle = -spawn->angle;
            state->soundHandle = func_800D3F64(0x565, func_800D3FD8());
            break;
        case 1:
            state->angle = 0;
            break;
        case 2:
            state->angle = spawn->angle;
            break;
        }
        pool = D_800F33E0->pool;
        return func_800CE560(pool, 12, 16, ROOMEFFECT_ORBIT_FLARE_CALLBACK);
    case 1:
        state->frame++;
        switch (state->state) {
        case 0:
            if (spawn->attached) {
                func_800CE8F0(D_800F32D0->pool, 0, &rotation, state);
            } else {
                func_800CE8F0(D_800F32D0->pool, 0x19, &rotation, state);
            }
            height = rsin(((s16)state->frame << 10) / 52) * 3 / 2;
            state->height = height;
            state->spread = (s16)height / 48;
            if (D_800E27EC % 3 == 0) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = state->x;
                    child->y = state->y;
                    child->z = state->z;
                    child->radius = state->height;
                }
            }
            if ((s16)state->frame < 52) break;
            state->state = 1;
            state->frame = 0;
            func_800CE9D4(D_800F32D0->pool, 0, &spin);
            state->angle += 0x400 - spin.y;
            if (D_800E2368->active) {
                RoomFlareNode **slot = (RoomFlareNode **)D_800F32D0->pool;
                if (slot && *slot) {
                    u8 *flag = (*slot)->state;
                    if (*flag == 1) *flag = 2;
                }
            }
            break;
        case 1:
            state->x += rcos(state->angle) * spawn->radius / 4096;
            state->z += rsin(state->angle) * spawn->radius / 4096;
            state->height = 0x1000;
            if (D_800E27EC & 1) {
                state->spread = 0x80;
            } else {
                state->spread = 0x60;
            }
            if (D_800E27EC & 1) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = state->x;
                    child->y = state->y;
                    child->z = state->z;
                    child->radius = state->height;
                }
            }
            if (func_8001CAB0(state->x << 16, state->z << 16, D_8009D248,
                              D_8009D1CC) == 0) {
                state->state = 2;
                state->frame = 0;
                if (state->soundHandle != -1) {
                    func_800866A4(state->soundHandle, 0);
                }
            }
            if (func_800C6B90(state, 0x39) == 0) break;
            if (D_800E2368->active) {
                ROOMEFFECT_ORBIT_FLARE_FLASH();
            }
            state->state = 2;
            state->frame = 0;
            if (state->soundHandle != -1) {
                func_800866A4(state->soundHandle, 0);
            }
            break;
        case 2:
            state->y--;
            if ((s16)state->frame < 16) break;
            return 1;
        }
        break;
    case 2:
    {
        int tpage;
        /* The texture slot is read as an element of the slot table so the
         * load stays below the first parameter store, and every draw reads
         * its palette back from the block (the first one folds to
         * D_800E1204[2]); that read-back is what keeps the block address
         * in s1 and lets the spin address be recomputed after GetClut. */
        D_800F3368.parameter00 = 0x10;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 0x10;
        D_800F3368.extent_y = 0x10;
        tpage = D_800E2850[D_800E11E4[2]];
        D_800F3368.palette = 2;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        D_800F3368.tpage = tpage;
        switch (state->state) {
        case 0:
        case 1:
            {
            u16 clut;
            int kind;
            int palette;
            spin.z = D_800E27EC << 7;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = GetClut(0x40, palette);
            func_800CEE20(state, &spin, state->height, state->height,
                          D_800F3368.parameter02 * 2 + 0xDC, clut, 1,
                          state->spread, 0);
            }
            {
            u16 clut;
            int kind;
            int palette;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = GetClut(0x40, palette);
            func_800CEE20(state, 0, state->height * 2, state->height * 2,
                          D_800F3368.parameter02 * 2 + 0xD8, clut, 3,
                          state->spread, 0);
            }
            {
            u16 clut;
            int kind;
            int palette;
            floorSpin.x = 0x400;
            floorSpin.y = 0;
            floorSpin.z = 0;
            floorSpin.flags = 1;
            floorPos.x = state->x;
            floorPos.z = state->z;
            floorPos.y = D_800942EC.count;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = GetClut(0x40, palette);
            func_800CEE20(&floorPos, &floorSpin, state->height, state->height,
                          D_800F3368.parameter02 * 2 + 0xD8, clut, 3,
                          state->spread, 0);
            }
            break;
        case 2:
            {
            u16 clut;
            int kind;
            int palette;
            scale = rsin((s16)state->frame << 6) + 0x800;
            fade = rcos((s16)state->frame << 6) / 32;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = GetClut(0x40, palette);
            func_800CEE20(state, 0, scale, scale,
                          D_800F3368.parameter02 + 0xFD, clut, 1, fade, 0);
            }
            {
            u16 clut;
            int kind;
            int palette;
            floorSpin2.x = 0x400;
            floorSpin2.y = 0;
            floorSpin2.z = 0;
            floorSpin2.flags = 1;
            floorPos2.x = state->x;
            floorPos2.z = state->z;
            floorPos2.y = D_800942EC.count;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = GetClut(0x40, palette);
            func_800CEE20(&floorPos2, &floorSpin2, scale, scale,
                          D_800F3368.parameter02 + 0xFD, clut, 3, fade, 0);
            }
            break;
        }
        {
            int index2 = D_800E11E8;
            int tpage2;
            D_800F3368.parameter00 = 0x10;
            D_800F3368.parameter02 = 1;
            D_800F3368.extent_x = 0x10;
            D_800F3368.extent_y = 0x10;
            tpage2 = D_800E2850[index2];
            D_800F3368.palette = 2;
            D_800F3368.parameter06 = 0;
            D_800F3368.tpage = tpage2;
        }
        break;
    }
    }
    return 0;
}

#undef ROOMEFFECT_ORBIT_FLARE_FLASH
#undef ROOMEFFECT_ORBIT_FLARE_FUNC
#undef ROOMEFFECT_ORBIT_FLARE_ROTATION
#undef ROOMEFFECT_ORBIT_FLARE_SPIN
#undef ROOMEFFECT_ORBIT_FLARE_CALLBACK
