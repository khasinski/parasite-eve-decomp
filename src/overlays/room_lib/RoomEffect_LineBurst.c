/* MASPSX_FLAGS: --expand-div */
/*
 * The line burst: a controller anchored on the actor that spits bounce
 * particles, sweeps a line from its anchor to a sprite point, rings it with
 * particles, holds a shrinking glow that tags the player and fades out.
 *
 * Fourteen room overlays link the same two functions in this order, with the
 * particle colour, the anchor template and the sprite rotation as the only
 * read-only data; this unit is that object, compiled into each of them.
 */
#include "pe1/room_line_burst.h"
#include "pe1/gte.h"

static const RoomLineBurstColor s_LineBurstParticleColor = { 0x80, 0x80, 0x80, 0 };
static const RoomLineBurstWords8 s_LineBurstAnchorTemplate = { { 0, 0xFF88 } };
static const RoomLineBurstWords8 s_LineBurstSpriteRotation = { { 0x400, 0x10000 } };

/* The burst's bounce particle. */
#define ROOMLIB_UPDATE_BOUNCE_RENDER_NAME RoomEffect_LineBurstParticle
#define ROOMLIB_BOUNCE_RENDER_BLOB s_LineBurstParticleColor
#include "RoomLib_UpdateBounceRender.inc"

/* Four-phase burst anchored on the actor: a pair of bounce particles and a
 * line swept from the anchor to the sprite point, a ring of eight particles
 * once the line arrives, a shrinking glow that tags the player while it
 * holds, then a fade.  Mode 2 projects the line through RTPT into a LINE_F2
 * packet and draws the glow sprite. */
int RoomEffect_LineBurstController(int mode, RoomLineBurstState *state,
                                   RoomLineBurstParams *params) {
    RoomLineBurstWords8 template = s_LineBurstAnchorTemplate;
    RoomLineBurstWords8 rotation = s_LineBurstSpriteRotation;
    s16 target[4];
    int otz;
    int weight;
    RoomLineBurstParticle *child;
    int angle;
    int i;

    switch (mode) {
    case 0:
        state->state = 0;
        state->timer = 0;
        state->scale = 0;
        state->intensity = 0;
        state->countdown = 0;
        state->px = params->x;
        state->py = params->y;
        state->pz = params->z;
        state->py = g_RoomFloorY->raw;
        func_800D3F64(0x57B, func_800D3FD8());
        if (D_800E2368->active) {
            RoomSparkNode **slot = (RoomSparkNode **)D_800F32D0->pool;
            if (slot && *slot) {
                u8 *flag = (*slot)->state;
                if (*flag == 1) *flag = 2;
            }
        }
        return func_800CE560(D_800F33E0->pool, 0x14, 0x18,
                             (FieldAnimCallbackListCallback)RoomEffect_LineBurstParticle);
    case 1:
        func_800CE8F0(D_800F32D0->pool, 0x13, &template, state);
        func_800CE9D4((struct RoomFxTransformOwner *)D_800F32D0->pool, 0,
                      (GteShortVector *)target);
        angle = -target[1] + 0x400;
        switch (state->state) {
        case 0:
            state->timer++;
            if (state->timer == 1) {
                for (i = 0; i < 2; i++) {
                    child = func_800CE610(D_800F33E0->pool);
                    if (child) {
                        child->x = state->x;
                        child->y = state->y;
                        child->z = state->z;
                        child->vx = rcos(angle) / 256;
                        child->vz = rsin(angle) / 256;
                        child->vy = 0;
                        child->state = i;
                        child->timer = 0;
                    }
                }
            }
            weight = 0x1000 - rcos((state->timer << 10) / 6);
            LoadAverageShort12(state, &state->px, 0x1000 - weight, weight, &state->ex);
            state->color = 0x80 - (state->timer << 5) / 6;
            if (state->timer < 6) break;
            state->state = 1;
            state->timer = 0;
            for (i = 0; i < 8; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    int speed;
                    child->x = state->px;
                    child->y = state->py;
                    child->z = state->pz;
                    speed = (func_80071A54() & 7) + 0x14;
                    child->vx = rcos(angle) * speed / 4096;
                    child->vz = rsin(angle) * speed / 4096;
                    child->vy = 0;
                    child->state = 0;
                    child->timer = 0;
                }
                angle += 0x200;
            }
            if (func_8001CAB0(state->px << 16, state->pz << 16, D_8009D248,
                              D_8009D1CC)) {
                break;
            }
            return 1;
        case 1:
            state->color = rcos((++state->timer << 10) / 6) / 128 + 0x40;
            state->ex = state->px;
            state->ey = state->py;
            state->ez = state->pz;
            state->scale = rsin((state->timer << 10) / 6) + 0x800;
            state->intensity = 0x80;
            if (state->timer < 6) break;
            state->state = 2;
            state->timer = 0;
            state->color = 0;
            for (i = 0; i < 12; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    weight = func_80071A54() & 0xFFF;
                    LoadAverageShort12(state, &state->px, 0x1000 - weight, weight, child);
                    child->vy = 0;
                    child->ay = (func_80071A54() & 3) + 1;
                    child->state = 1;
                    child->timer = 0;
                }
            }
            break;
        case 2:
            state->timer++;
            if (state->scale > 0x1000) state->scale -= 0x400;
            state->intensity = rsin((state->timer << 14) / params->duration) / 128 + 0x80;
            if (state->countdown != 0) state->countdown--;
            if (func_800C6B90(&state->px, 0x104) && state->countdown == 0) {
                if (D_800E2368->active) {
                    RoomSparkChannel *channel = D_800F32D0;
                    u32 *entry;
                    if ((**(u32 **)channel->pool & 0x3F000000) == 0x01000000) {
                        D_8009D254->actor->flags |= 0x4000;
                        entry = *(u32 **)channel->pool;
                        *entry = (*entry & 0xC0FFFFFF) | 0x19000000;
                        entry = *(u32 **)channel->pool;
                        *entry |= 0x80000000;
                    }
                }
                state->countdown = params->countdown;
            }
            if (state->timer < params->duration) break;
            state->state = 3;
            state->timer = 0;
            break;
        case 3:
            state->scale = rcos(++state->timer << 5);
            state->intensity = rcos(state->timer << 5) / 32;
            if (state->timer >= 0x20) return 1;
            break;
        }
        break;
    case 2: {
        RenderMatrixSlot *matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        if (state->color != 0) {
            RoomLineBurstLinePacket *line =
                (RoomLineBurstLinePacket *)(D_800B0E58[D_8009CDDC] + D_8009CDD8);
            D_8009CDD8 += 0x10;
            gte_ldv3(state, &state->ex, state);
            gte_rtpt_padded();
            line->length = 3;
            line->code = 0x40;
            line->r = state->color;
            line->g = state->color;
            line->b = state->color;
            gte_stszotz(&otz);
            if ((u32)(otz - 1) < 0xFFF) {
                gte_stsxy3(&line->x0, &line->x1, &weight);
                func_800CF6F8(D_800B0E58[D_8009CDDC - 8] + otz * 4, line, 1);
            }
        }
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11EC];
            D_800F3368.palette = 4;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 5;
            D_800F3368.depth = 0;
            D_800F3368.tpage = tpage;
        }
        if (state->intensity != 0) {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            u16 clut;
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0, palette);
            func_800CEE20((GteShortVector *)&state->px, (GteRotation *)&rotation,
                          state->scale, state->scale, 0, clut, 1,
                          state->intensity / 2, 0);
        }
        D_800F3368.parameter00 = 0x10;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 0x10;
        D_800F3368.extent_y = 0x10;
        {
            int tpage = D_800E2850[D_800E11E8];
            D_800F3368.palette = 2;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 8;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    }
    return 0;
}
