/* MASPSX_FLAGS: --expand-div */
/*
 * Trail and burst attacks: two seeking trails that chase the player, each
 * shedding sparks and flashes, and two model bursts that grow a scaled model
 * at one of the actor's joints while shedding pulsing sprites.
 *
 * room_m245 and room_m397 link these eight functions and the two trail
 * classes' history getters in this order, with the same 0x18 bytes of
 * read-only seeds; this unit is that object, compiled into each of them. The second trail takes its history slot from the event
 * state and scatters its heading with a fixed spread; the second burst
 * follows another joint. The trail histories, colour ramps, model assets,
 * layer records and anchors live in each room's own data.
 */
#include "pe1/room_trail_burst.h"
#include "pe1/gte.h"

static const RenderColor s_TrailHeadColor = { 0xA0, 0xA0, 0xA0, 0 };
static const RenderColor s_TrailTailColor = { 0x00, 0x00, 0x20, 0 };
static const GteRotation s_JointOffset = { 0, 0, -250, 0 };
static const RenderColor s_PulseColor = { 0x80, 0x80, 0x8C, 0 };
static const RenderColor s_BurstShade = { 0x00, 0x00, 0xC8, 0 };

/* First object of the primary channel's pool: its packet word. */
typedef struct RoomSeekingTrailPacket {
    u32 code;
} RoomSeekingTrailPacket;

/* Seeking trail: state 1 is the head that turns towards the player and
 * records its path, state 2 a spark shed from the path and state 3 a flash
 * at the head. A trail point near the player flags the battle actor. */
int RoomEffect_SeekingTrailParticle(int mode, RoomSeekingTrail *trail) {
    RenderColor colorA;
    RenderColor colorB;
    GteRotation rotation;
    GteShortVector target;
    GteShortVector angles;
    GteShortVector unused;
    RoomSeekingTrail *child;
    GteShortVector *point;
    int range;
    int i;
    int scale;
    int kind;
    int palette;
    u16 clut;
    u16 *palettes;

    colorA = s_TrailHeadColor;
    colorB = s_TrailTailColor;
    rotation = s_JointOffset;
    switch (mode) {
    case 1:
        switch (trail->state) {
        case 0:
            trail->timer = 0;
            trail->state = 1;
            trail->fade = 0;
            func_800CE8F0(D_800F32D0->pool, 7, &rotation, trail);
            func_800CE9D4((struct RoomFxTransformOwner *)D_800F32D0->pool, 0, &angles);
            angles.x = 0;
            trail->ax = angles.x;
            trail->ay = angles.y;
            trail->az = angles.z;
            func_800D3AFC(&g_RoomSeekingTrailHistory[trail->slot], 0x18, trail, 1);
            trail->speed = 0x20;
            trail->turn = 0x200;
            child = func_800CE610(D_800F33E0->pool);
            if (!child) return 0;
            child->x = trail->x;
            child->y = trail->y;
            child->z = trail->z;
            child->state = 3;
            child->timer = 0;
            break;
        case 1:
            trail->timer++;
            point = g_RoomSeekingTrailHistory[trail->slot].point;
            func_800CE870((char *)D_8009D254, 1, &target.x);
            func_800CFAA8(trail, &target, &angles);
            angles.x = 0;
            if (trail->fade == 0) {
                func_800CFD50(&angles, &trail->ax, trail->turn);
                if (trail->timer >= 3) {
                    range = trail->timer * 16 + 0x100;
                    if (!(func_80071A54() & 1)) {
                        trail->ay += func_80071A54() % (range * 2) - range;
                    } else {
                        trail->ay += func_80071A54() % range - (range >> 1);
                    }
                }
                if (trail->speed < 0x3C) trail->speed += 2;
                func_800CFB7C(&trail->ax, (s16)(trail->speed * trail->speed / 16), &target);
                trail->x += target.x;
                trail->y += target.y;
                trail->z += target.z;
            } else if (++trail->fade >= 16) {
                return 1;
            }
            func_800D3AFC(point, 0x18, trail, 0);
            point = g_RoomSeekingTrailHistory[trail->slot].point;
            for (i = 0; i < 0x18; i++, point++) {
                if (func_800C6B90(point, 0x68) && D_800E2368->active) {
                    RoomSparkChannel *channel = D_800F32D0;
                    if (((*(RoomSeekingTrailPacket **)channel->pool)->code & 0x3F000000) ==
                        0x01000000) {
                        D_8009D254->actor->flags |= 0x4000;
                        (*(RoomSeekingTrailPacket **)channel->pool)->code |= 0x3F000000;
                        (*(RoomSeekingTrailPacket **)channel->pool)->code |= 0x80000000;
                    }
                }
            }
            point = g_RoomSeekingTrailHistory[trail->slot].point;
            if (trail->fade == 0) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    point += (func_80071A54() & 3) * 4;
                    child->x = point->x;
                    child->y = point->y;
                    child->z = point->z;
                    child->x += -0x20 + (func_80071A54() & 0x3F);
                    child->y += -0x20 + (func_80071A54() & 0x3F);
                    child->z += -0x20 + (func_80071A54() & 0x3F);
                    child->ax = trail->ax;
                    child->ay = trail->ay;
                    child->az = trail->az;
                    child->ax += -100 + func_80071A54() % 200;
                    child->ay += -100 + func_80071A54() % 200;
                    child->speed = (func_80071A54() & 0x3F) - 0x20;
                    child->turn = (func_80071A54() & 0x3F) - 0x20;
                    child->state = 2;
                    child->timer = 0;
                }
            }
            if (D_800E27EC < 0x20) return 0;
            if (trail->fade != 0) return 0;
            child = func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = trail->x;
                child->y = trail->y;
                child->z = trail->z;
                child->state = 3;
                child->timer = 0;
            }
            trail->fade = 1;
            break;
        case 2:
            trail->timer++;
            trail->ax += trail->turn;
            trail->ay += trail->speed;
            if (trail->timer >= 20) return 1;
            break;
        case 3:
            if (++trail->timer < 24) break;
            return 1;
        default:
            return 0;
        }
        break;
    case 2:
        switch (trail->state) {
        case 1:
            scale = ((D_800E27EC / 3) & 1) * 32 + 32;
            i = 0;
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            {
                u16 kind = D_800F336C;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
            }
            clut = func_80077AA4(0x10, palette);
            func_800D3114(&g_RoomSeekingTrailHistory[trail->slot], 0x18, 0x78, i, scale,
                          0xFD, 0x1F, D_800E2850[D_800E11EA], clut, 0x80,
                          &colorA, &colorB, 1);
            func_800D004C(&g_RoomSeekingTrailHistory[trail->slot].point[func_80071A54() & 0xF],
                          0x190, 0x190, 6, 0, 0x1000, 0x1000, &colorA, 0, 0x80, 3);
            return 0;
        case 2:
            scale = rcos((trail->timer << 10) / 20) / 32;
            rcos((trail->timer << 10) / 20);
            /* Shares the loop index's register with retail (s0). */
            i = (D_800E27EC & 1) << 7;
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            /* tpage is the first parameter store, so its address stays in a
             * register and parameter00 is later written through it. */
            D_800F3368.tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800D2370((GteShortVector *)trail, (GteRotation *)&trail->ax, 0x1FE, 0x78,
                          i, 0, 0x80, 0x20, clut, &colorB, &colorA, (s16)scale, 1);
            palettes = D_800E1204;
            if (trail->timer < 13) {
                /* Block-scope temporaries: the kind read stays below the
                 * parameter00 store, as in retail. */
                int kind;
                int palette;

                scale = rcos((trail->timer << 10) / 12) / 64;
                D_800F3368.parameter00 = 0x20;
                D_800F336A = 2;
                D_800F3368.extent_x = 0x20;
                D_800F3368.extent_y = 0x20;
                kind = D_800F3368.palette;
                palette = palettes[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0x40, palette);
                func_800CEE20((GteShortVector *)trail, 0, 0x1000, 0x1000,
                              D_800F336A * (trail->timer * 2 / 3) + 0x90, clut, 1,
                              scale, 0);
            }
            break;
        case 3:
            scale = rcos((trail->timer << 10) / 24) / 32;
            rcos((trail->timer << 10) / 24);
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            target.x = trail->x;
            target.y = trail->y;
            target.z = trail->z;
            target.x += -0x10 + (func_80071A54() & 0x1F);
            target.y += -0x10 + (func_80071A54() & 0x1F);
            target.z += -0x10 + (func_80071A54() & 0x1F);
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x40, palette);
            func_800CEE20(&target, 0, 0x1000, 0x1000,
                          D_800F336A * (trail->timer & 7) + 0x90, clut, 1, scale, 0);
            break;
        default:
            return 0;
        }
        break;
    default:
        return 0;
    }
    return 0;
}

/* Trail controller: mode 0 opens a pool of 32 trails, mode 1 launches one
 * on the first frame, into the history slot the caller gives, and finishes
 * after eight frames; mode 2 sets the sprite parameters. */
int RoomEffect_SeekingTrailController(int mode, u16 *slot) {
    RoomSeekingTrail *trail;
    RoomSparkEventState *event;
    u16 tpage;

    switch (mode) {
    case 0:
        event = D_800E2368;
        *slot = 0;
        if (event->active != 0) {
            RoomSparkNode **nodes = (RoomSparkNode **)D_800F32D0->pool;
            if (nodes != 0 && *nodes != 0) {
                u8 *state = (*nodes)->state;
                if (*state == 1) {
                    *state = 2;
                }
            }
        }
        return func_800CE560(D_800F33E0->pool, 0x18, 0x20,
                             (FieldAnimCallbackListCallback)RoomEffect_SeekingTrailParticle);
    case 1:
        if (D_800E27EC == mode) {
            trail = func_800CE610(D_800F33E0->pool);
            if (trail != 0) {
                trail->slot = *slot;
                trail->state = 0;
                trail->timer = 0;
            }
        }
        if (D_800E27EC < 8) {
            break;
        }
        return 2;
    case 2:
        tpage = D_800E2850[D_800E11EA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        D_800F3368.tpage = tpage;
        break;
    }
    return 0;
}

/* Sprite shed by the model burst. Mode 1 drifts it for sixteen frames;
 * mode 2 draws a textured rectangle and a scaled sprite at the burst's
 * anchor, pulsing with a cosine of its frame counter. */
int RoomEffect_ModelBurstParticle(int mode, RoomModelBurstParticle *state) {
    RenderColor color;
    RenderMatrixSlot *matrixSlot;
    int scale;
    int kind;
    int palette;
    int u;
    u16 clut;

    color = s_PulseColor;
    switch (mode) {
    case 1:
        state->frame++;
        state->x += state->vx;
        state->y += state->vy;
        state->z += 0x18;
        if ((short)state->frame >= 16) return 1;
        break;
    case 2:
        scale = rcos((short)state->frame << 6) / 32;
        rcos((short)state->frame << 6);
        u = (D_800E27EC & 1) << 7;
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428 != 0) palette += 4;
        clut = func_80077AA4(0x10, palette);
        func_800D2370(&g_RoomModelBurstAnchor, (GteRotation *)state, 0x1FE, 0x6E, u, 0,
                      0x80, 0x20, clut, &color, &color, (short)scale, 1);
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428 != 0) palette += 4;
        clut = func_80077AA4(0x40, palette);
        func_800CEE20(&g_RoomModelBurstAnchor, 0, 0x1000, 0x1000,
                      D_800F336A * ((short)state->frame / 2) + 0x90, clut, 1, scale / 2, 0);
        break;
    }
    return 0;
}

/* Model burst: loads the model asset, then for 48 frames brightens and sheds
 * one sprite per frame, while drawing a growing sprite pair at joint 7 and
 * the spinning, scaled model coloured from the room's ramp. */
int RoomEffect_ModelBurstController(int mode, s16 *state) {
    GteRotation rotation = s_JointOffset;
    GteShortVector position;
    GteRotation spin;
    RenderColor color;
    RenderColor shade = s_BurstShade;
    GteMatrix matrix;
    GteVector scale;
    RoomModelBurstParticle *child;
    u16 *tpages;
    int brightness;
    int size;
    int handle;

    switch (mode) {
    case 0:
        *state = 0;
        g_RoomModelBurstAsset = func_8006E498(D_800B0E64, 0xC54A5704);
        func_800C6D5C(g_RoomModelBurstAsset, 0, 0);
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.tpage = tpage;
        }
        func_800CF4B4(3, -1, g_RoomModelBurstLayer);
        handle = func_800D3FD8();
        func_800D3F64(0x5A4, handle);
        return func_800CE560(D_800F33E0->pool, 0x10, 0x10,
                             (FieldAnimCallbackListCallback)RoomEffect_ModelBurstParticle);
    case 1:
        if (*state < 0x80) *state += 8;
        if (D_800E27EC >= 2) {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.tpage = tpage;
            func_800CF4B4(3, D_800E27EC * 2, g_RoomModelBurstLayer);
        }
        child = func_800CE610(D_800F33E0->pool);
        if (child) {
            child->x = func_80071A54();
            child->y = func_80071A54();
            child->z = func_80071A54();
            child->frame = 0;
            child->reserved08 = 0;
            child->vx = (func_80071A54() & 0x7F) - 0x40;
            child->vy = (func_80071A54() & 0x7F) - 0x40;
        }
        if (D_800E27EC < 0x30) break;
        return 1;
    case 2:
        {
            int tpage;
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            tpage = D_800E2850[D_800E11FA];
            D_800F3368.parameter06 = 1;
            D_800F3368.palette = 3;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0x18;
            D_800F3368.tpage = tpage;
        }
        func_800CE8F0(D_800F32D0->pool, 7, &rotation, &position);
        spin.x = rsin(D_800E27EC * 80) / 4;
        spin.y = rcos(D_800E27EC << 7) / 8;
        spin.flags = 0;
        spin.z = -D_800E27EC << 3;
        brightness = *state;
        if (D_800E27EC != 0) brightness = brightness * 2 / 3;
        size = rsin((D_800E27EC << 10) / 48) / 4 + 0x400;
        func_800CF3AC(g_RoomModelBurstColors, &color, D_800E27EC * 3);
        func_800D004C(&position, 0x124, 0x124, 12, 0, 0x1000, 0x1000, &shade, 0,
                      brightness, 1);
        func_800D0728(&position, 0xC8, 0x1F4, 12, 0, 0x1000, 0x1000, &shade, 0,
                      brightness / 2, 1);
        g_RoomModelBurstAnchor.x = position.x;
        g_RoomModelBurstAnchor.y = position.y;
        g_RoomModelBurstAnchor.z = position.z;
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        func_800CF3AC(g_RoomModelBurstColors, &color, D_800E27EC);
        {
            u16 clut;
            int kind;
            int palette;
            int page;
            int tpage;
            tpages = D_800E2850;
            tpage = tpages[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.tpage = tpage;
            page = (tpages[D_800E11EA] | func_80077A64(0, 1, 0, 0)) & 0xFFFF;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x30, palette);
            func_800C6EC0(page, clut);
        }
        func_800C6ED8(1);
        RotMatrixYXZ((GteShortVector *)&spin, &matrix);
        matrix.t[0] = position.x;
        matrix.t[1] = position.y;
        matrix.t[2] = position.z;
        scale.x = size;
        scale.y = size;
        scale.z = size;
        ScaleMatrix(&matrix, &scale);
        func_800C6EF8(g_RoomModelBurstAsset);
        func_800C7098(g_RoomModelBurstAsset, color.r, color.g, color.b);
        func_800C6FA0(g_RoomModelBurstAsset, (u16)(brightness / 2));
        func_800C71E4(g_RoomModelBurstAsset, &matrix);
        func_800C6F4C(g_RoomModelBurstAsset);
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.depth = 8;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}

/* The second trail: it takes its history slot from the event state, nudges
 * its initial heading, starts from joint 5 and jitters with a fixed spread. */
int RoomEffect_ScatterTrailParticle(int mode, RoomSeekingTrail *trail) {
    RenderColor colorA;
    RenderColor colorB;
    GteRotation rotation;
    GteShortVector target;
    GteShortVector angles;
    RoomSeekingTrail *child;
    GteShortVector *point;
    int i;
    int scale;
    int kind;
    int palette;
    u16 clut;
    u16 *palettes;

    colorA = s_TrailHeadColor;
    colorB = s_TrailTailColor;
    rotation = s_JointOffset;
    switch (mode) {
    case 1:
        switch (trail->state) {
        case 0:
            trail->timer = 0;
            trail->state = 1;
            trail->slot = D_800E2368->trailSlot;
            trail->fade = 0;
            func_800CE8F0(D_800F32D0->pool, 5, &rotation, trail);
            func_800CE9D4((struct RoomFxTransformOwner *)D_800F32D0->pool, 0, &angles);
            angles.x = 0;
            angles.y += (func_80071A54() & 0x1FF) - 0x100;
            trail->ax = angles.x;
            trail->ay = angles.y;
            trail->az = angles.z;
            func_800D3AFC(&g_RoomScatterTrailHistory[trail->slot], 0x18, trail, 1);
            trail->speed = 0x20;
            trail->turn = 0x200;
            child = func_800CE610(D_800F33E0->pool);
            if (!child) return 0;
            child->x = trail->x;
            child->y = trail->y;
            child->z = trail->z;
            child->state = 3;
            child->timer = 0;
            break;
        case 1:
            trail->timer++;
            point = g_RoomScatterTrailHistory[trail->slot].point;
            func_800CE870((char *)D_8009D254, 1, &target.x);
            func_800CFAA8(trail, &target, &angles);
            angles.x = 0;
            if (trail->fade == 0) {
                func_800CFD50(&angles, &trail->ax, trail->turn);
                if (!(func_80071A54() & 1)) {
                    trail->ay += func_80071A54() % 800 - 400;
                } else {
                    trail->ay += (func_80071A54() & 0x1FF) - 0x100;
                }
                if (trail->speed < 0x3C) trail->speed += 2;
                func_800CFB7C(&trail->ax, (s16)(trail->speed * trail->speed / 16), &target);
                trail->x += target.x;
                trail->y += target.y;
                trail->z += target.z;
            } else if (++trail->fade >= 16) {
                return 1;
            }
            func_800D3AFC(point, 0x18, trail, 0);
            point = g_RoomScatterTrailHistory[trail->slot].point;
            for (i = 0; i < 0x18; i++, point++) {
                if (func_800C6B90(point, 0x64) && D_800E2368->active) {
                    RoomSparkChannel *channel = D_800F32D0;
                    if (((*(RoomSeekingTrailPacket **)channel->pool)->code & 0x3F000000) ==
                        0x01000000) {
                        D_8009D254->actor->flags |= 0x4000;
                        (*(RoomSeekingTrailPacket **)channel->pool)->code |= 0x3F000000;
                        (*(RoomSeekingTrailPacket **)channel->pool)->code |= 0x80000000;
                    }
                }
            }
            point = g_RoomScatterTrailHistory[trail->slot].point;
            if (trail->fade == 0) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    point += (func_80071A54() & 3) * 4;
                    child->x = point->x;
                    child->y = point->y;
                    child->z = point->z;
                    child->x += -0x20 + (func_80071A54() & 0x3F);
                    child->y += -0x20 + (func_80071A54() & 0x3F);
                    child->z += -0x20 + (func_80071A54() & 0x3F);
                    child->ax = trail->ax;
                    child->ay = trail->ay;
                    child->az = trail->az;
                    child->ax += -100 + func_80071A54() % 200;
                    child->ay += -100 + func_80071A54() % 200;
                    child->speed = (func_80071A54() & 0x3F) - 0x20;
                    child->turn = (func_80071A54() & 0x3F) - 0x20;
                    child->state = 2;
                    child->timer = 0;
                }
            }
            if (D_800E27EC < 0x20) return 0;
            if (trail->fade != 0) return 0;
            child = func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = trail->x;
                child->y = trail->y;
                child->z = trail->z;
                child->state = 3;
                child->timer = 0;
            }
            trail->fade = 1;
            break;
        case 2:
            trail->timer++;
            trail->ax += trail->turn;
            trail->ay += trail->speed;
            if (trail->timer >= 20) return 1;
            break;
        case 3:
            if (++trail->timer < 24) break;
            return 1;
        default:
            return 0;
        }
        break;
    case 2:
        switch (trail->state) {
        case 1:
            scale = ((D_800E27EC / 2) & 1) * 32 + 32;
            i = 0;
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            {
                u16 kind = D_800F336C;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
            }
            clut = func_80077AA4(0x10, palette);
            func_800D3114(&g_RoomScatterTrailHistory[trail->slot], 0x18, 0x78, i, scale,
                          0xFD, 0x1F, D_800E2850[D_800E11EA], clut, 0x80,
                          &colorA, &colorB, 1);
            func_800D004C(&g_RoomScatterTrailHistory[trail->slot].point[func_80071A54() & 0xF],
                          0x190, 0x190, 6, 0, 0x1000, 0x1000, &colorA, 0, 0x80, 3);
            return 0;
        case 2:
            scale = rcos((trail->timer << 10) / 20) / 32;
            rcos((trail->timer << 10) / 20);
            /* Shares the loop index's register with retail (s0). */
            i = (D_800E27EC & 1) << 7;
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            /* tpage is the first parameter store, so its address stays in a
             * register and parameter00 is later written through it. */
            D_800F3368.tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800D2370((GteShortVector *)trail, (GteRotation *)&trail->ax, 0x1FE, 0x6E,
                          i, 0, 0x80, 0x20, clut, &colorB, &colorA, (s16)scale, 1);
            palettes = D_800E1204;
            if (trail->timer < 13) {
                /* Block-scope temporaries: the kind read stays below the
                 * parameter00 store, as in retail. */
                int kind;
                int palette;

                scale = rcos((trail->timer << 10) / 12) / 64;
                D_800F3368.parameter00 = 0x20;
                D_800F336A = 2;
                D_800F3368.extent_x = 0x20;
                D_800F3368.extent_y = 0x20;
                kind = D_800F3368.palette;
                palette = palettes[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0x40, palette);
                func_800CEE20((GteShortVector *)trail, 0, 0x1000, 0x1000,
                              D_800F336A * (trail->timer * 2 / 3) + 0x90, clut, 1,
                              scale, 0);
            }
            break;
        case 3:
            scale = rcos((trail->timer << 10) / 24) / 32;
            rcos((trail->timer << 10) / 24);
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            target.x = trail->x;
            target.y = trail->y;
            target.z = trail->z;
            target.x += -0x10 + (func_80071A54() & 0x1F);
            target.y += -0x10 + (func_80071A54() & 0x1F);
            target.z += -0x10 + (func_80071A54() & 0x1F);
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x40, palette);
            func_800CEE20(&target, 0, 0x1000, 0x1000,
                          D_800F336A * (trail->timer & 7) + 0x90, clut, 1, scale, 0);
            break;
        default:
            return 0;
        }
        break;
    default:
        return 0;
    }
    return 0;
}

/* Controller of the second trail, as the first. */
int RoomEffect_ScatterTrailController(int mode, u16 *slot) {
    RoomSeekingTrail *trail;
    RoomSparkEventState *event;
    u16 tpage;

    switch (mode) {
    case 0:
        event = D_800E2368;
        *slot = 0;
        if (event->active != 0) {
            RoomSparkNode **nodes = (RoomSparkNode **)D_800F32D0->pool;
            if (nodes != 0 && *nodes != 0) {
                u8 *state = (*nodes)->state;
                if (*state == 1) {
                    *state = 2;
                }
            }
        }
        return func_800CE560(D_800F33E0->pool, 0x18, 0x20,
                             (FieldAnimCallbackListCallback)RoomEffect_ScatterTrailParticle);
    case 1:
        if (D_800E27EC == mode) {
            trail = func_800CE610(D_800F33E0->pool);
            if (trail != 0) {
                trail->slot = *slot;
                trail->state = 0;
                trail->timer = 0;
            }
        }
        if (D_800E27EC < 8) {
            break;
        }
        return 2;
    case 2:
        tpage = D_800E2850[D_800E11EA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        D_800F3368.tpage = tpage;
        break;
    }
    return 0;
}

/* Sprite shed by the second burst, drawn at its anchor. */
int RoomEffect_ScatterBurstParticle(int mode, RoomModelBurstParticle *state) {
    RenderColor color;
    RenderMatrixSlot *matrixSlot;
    int scale;
    int kind;
    int palette;
    int u;
    u16 clut;

    color = s_PulseColor;
    switch (mode) {
    case 1:
        state->frame++;
        state->x += state->vx;
        state->y += state->vy;
        state->z += 0x18;
        if ((short)state->frame >= 16) return 1;
        break;
    case 2:
        scale = rcos((short)state->frame << 6) / 32;
        rcos((short)state->frame << 6);
        u = (D_800E27EC & 1) << 7;
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428 != 0) palette += 4;
        clut = func_80077AA4(0x10, palette);
        func_800D2370(&g_RoomScatterBurstAnchor, (GteRotation *)state, 0x1FE, 0x64, u, 0,
                      0x80, 0x20, clut, &color, &color, (short)scale, 1);
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428 != 0) palette += 4;
        clut = func_80077AA4(0x40, palette);
        func_800CEE20(&g_RoomScatterBurstAnchor, 0, 0x1000, 0x1000,
                      D_800F336A * ((short)state->frame / 2) + 0x90, clut, 1, scale / 2, 0);
        break;
    }
    return 0;
}

/* The second burst, following joint 5. */
int RoomEffect_ScatterBurstController(int mode, s16 *state) {
    GteRotation rotation = s_JointOffset;
    GteShortVector position;
    GteRotation spin;
    RenderColor color;
    RenderColor shade = s_BurstShade;
    GteMatrix matrix;
    GteVector scale;
    RoomModelBurstParticle *child;
    u16 *tpages;
    int brightness;
    int size;
    int handle;

    switch (mode) {
    case 0:
        *state = 0;
        g_RoomScatterBurstAsset = func_8006E498(D_800B0E64, 0xC54A5704);
        func_800C6D5C(g_RoomScatterBurstAsset, 0, 0);
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.tpage = tpage;
        }
        func_800CF4B4(3, -1, g_RoomScatterBurstLayer);
        handle = func_800D3FD8();
        func_800D3F64(0x5A4, handle);
        return func_800CE560(D_800F33E0->pool, 0x10, 0x10,
                             (FieldAnimCallbackListCallback)RoomEffect_ScatterBurstParticle);
    case 1:
        if (*state < 0x80) *state += 8;
        if (D_800E27EC >= 2) {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.tpage = tpage;
            func_800CF4B4(3, D_800E27EC * 2, g_RoomScatterBurstLayer);
        }
        child = func_800CE610(D_800F33E0->pool);
        if (child) {
            child->x = func_80071A54();
            child->y = func_80071A54();
            child->z = func_80071A54();
            child->frame = 0;
            child->reserved08 = 0;
            child->vx = (func_80071A54() & 0x7F) - 0x40;
            child->vy = (func_80071A54() & 0x7F) - 0x40;
        }
        if (D_800E27EC < 0x30) break;
        return 1;
    case 2:
        {
            int tpage;
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            tpage = D_800E2850[D_800E11FA];
            D_800F3368.parameter06 = 1;
            D_800F3368.palette = 3;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0x18;
            D_800F3368.tpage = tpage;
        }
        func_800CE8F0(D_800F32D0->pool, 5, &rotation, &position);
        spin.x = rsin(D_800E27EC * 80) / 4;
        spin.y = rcos(D_800E27EC << 7) / 8;
        spin.flags = 0;
        spin.z = -D_800E27EC << 3;
        brightness = *state;
        if (D_800E27EC != 0) brightness = brightness * 2 / 3;
        size = rsin((D_800E27EC << 10) / 48) / 4 + 0x400;
        func_800CF3AC(g_RoomScatterBurstColors, &color, D_800E27EC * 3);
        func_800D004C(&position, 0x124, 0x124, 12, 0, 0x1000, 0x1000, &shade, 0,
                      brightness, 1);
        func_800D0728(&position, 0xC8, 0x1F4, 12, 0, 0x1000, 0x1000, &shade, 0,
                      brightness / 2, 1);
        g_RoomScatterBurstAnchor.x = position.x;
        g_RoomScatterBurstAnchor.y = position.y;
        g_RoomScatterBurstAnchor.z = position.z;
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        func_800CF3AC(g_RoomScatterBurstColors, &color, D_800E27EC);
        {
            u16 clut;
            int kind;
            int palette;
            int page;
            int tpage;
            tpages = D_800E2850;
            tpage = tpages[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.tpage = tpage;
            page = (tpages[D_800E11EA] | func_80077A64(0, 1, 0, 0)) & 0xFFFF;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x30, palette);
            func_800C6EC0(page, clut);
        }
        func_800C6ED8(1);
        RotMatrixYXZ((GteShortVector *)&spin, &matrix);
        matrix.t[0] = position.x;
        matrix.t[1] = position.y;
        matrix.t[2] = position.z;
        scale.x = size;
        scale.y = size;
        scale.z = size;
        ScaleMatrix(&matrix, &scale);
        func_800C6EF8(g_RoomScatterBurstAsset);
        func_800C7098(g_RoomScatterBurstAsset, color.r, color.g, color.b);
        func_800C6FA0(g_RoomScatterBurstAsset, (u16)(brightness / 2));
        func_800C71E4(g_RoomScatterBurstAsset, &matrix);
        func_800C6F4C(g_RoomScatterBurstAsset);
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.depth = 8;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}

/* History getters of the seeking and the scatter trail classes; retail has
 * both return the seeking trail's histories. */
void *RoomEffect_SeekingTrailHistory(void) {
    return g_RoomSeekingTrailHistory;
}

void *RoomEffect_ScatterTrailHistory(void) {
    return g_RoomSeekingTrailHistory;
}
