/*
 * The homing flash: a controller that anchors a flash to the actor, then
 * releases sparks that home on the player and flag the battle actor when
 * they arrive.
 *
 * room_m162, m163, m398, m400, m403, scene_e24 and scene_e25 link the same
 * two functions in this order, right after the room library, with the
 * spark's hit colour and the two rotation seeds as their only read-only
 * data; this unit is that object, compiled into each of them. The colour
 * ramp the controller blends through is room data (g_RoomFlashColorRamp),
 * named in each room's symbol file.
 */
#include "pe1/room_homing_flash.h"
#include "pe1/gte.h"

static const RoomSoundBurstColor s_HomingSparkHitColor = { 0xFF, 0x80, 0, 0 };
static const GteRotation s_HomingSparkRotation = { 0x400, 0, 0, 1 };
static const GteRotation s_FlashSpriteRotation = { 0, 0, -100, 0 };

/* Spark that homes on the player, flags the battle actor when it arrives,
 * then burns out; sub-states 1 and 2 play the hit and fade animations. */
int RoomEffect_HomingSpark(int mode, RoomHomingSpark *spark,
                           RoomHomingSparkParams *params) {
    RoomSoundBurstColor color = s_HomingSparkHitColor;
    GteRotation rotation = s_HomingSparkRotation;
    GteShortVector angles;
    GteShortVector target;
    RoomHomingSpark *child;
    char *pool;
    u16 clut;
    int fade;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
            func_800CE870(D_8009D254, 0, &target);
            func_800CFAA8(spark, &target, &angles);
            angles.x = 0;
            func_800CFD50(&angles, &spark->hx, params->speed);
            func_800CFB7C(&spark->hx, params->distance, &target);
            spark->x += target.x;
            spark->y += target.y;
            spark->z += target.z;
            if (func_800C6B90(spark, 50)) {
                if (D_800E2368->active) {
                    RoomSoundBurstChannel *channel = D_800F32D0;
                    u32 *entry;
                    if ((**(u32 **)channel->pool & 0x3F000000) == 0x01000000) {
                        D_8009D254->actor->flags |= 0x4000;
                        entry = *(u32 **)channel->pool;
                        *entry = (*entry & 0xC0FFFFFF) | 0x11000000;
                        entry = *(u32 **)channel->pool;
                        *entry |= 0x80000000;
                    }
                }
                spark->state = 1;
                spark->timer = 0;
                if (spark->soundHandle != -1) {
                    func_800866A4(spark->soundHandle, 0);
                }
            }
            if (D_800E27EC & 1) {
                pool = D_800F33E0->pool;
                child = (RoomHomingSpark *)func_800CE610(pool);
                if (child) {
                    child->x = spark->x;
                    child->y = spark->y;
                    child->z = spark->z;
                    child->state = 2;
                    child->timer = 0;
                }
            }
            if (D_800E27EC < 36) return 0;
            if (spark->soundHandle != -1) {
                func_800866A4(spark->soundHandle, 0);
            }
            return 1;
        case 1:
            spark->timer++;
            spark->y -= 8;
            if ((s16)spark->timer < 8) break;
            return 1;
        case 2:
            spark->timer++;
            if ((s16)spark->timer < 16) break;
            return 1;
        }
        break;
    case 2:
        switch (spark->state) {
        case 0: {
            int kind;
            int palette;
            fade = (D_800E27EC & 1) ? 0xBE : 0x80;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800CEE20(spark, &rotation, 0x1000, 0x1000, 0x20, clut, 1,
                          fade / 2, 0);
            break;
        }
        case 1: {
            int kind;
            int palette;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0, palette);
            func_800CEE20(spark, &rotation, 0x1000, 0x1000,
                          (s16)D_800F3368.parameter02 * spark->timer, clut, 1,
                          0x80, &color);
            break;
        }
        case 2: {
            int kind;
            int palette;
            fade = 0x29 - spark->timer * 41 / 16;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800CEE20(spark, &rotation, 0x1000, 0x1000, 0x20, clut, 1,
                          fade, 0);
            break;
        }
        }
        break;
    }
    return 0;
}

/* Controller that anchors a flash to the actor, plays a sound, waits sixteen
 * frames, releases two particles, then draws a pulsing sprite that blends
 * into the actor position. */
int RoomEffect_FlashSpriteController(int mode, RoomFlashSpriteState *state,
                                     RoomFlashSpriteSpawn *spawn) {
    GteRotation rotation = s_FlashSpriteRotation;
    GteShortVector target;
    GteShortVector output;
    RoomSoundBurstColor color;
    RoomFlashSpriteChild *child;
    RoomSoundBurstMatrixSlot *matrixSlot;
    char *pool;
    int handle;
    int kind;
    int palette;
    u16 clut;
    int scale;
    int blend;
    int opacity;
    int frame;
    int unit;

    switch (mode) {
    case 0:
        pool = D_800F32D0->pool;
        func_800CE9D4(pool, 0, &state->ax);
        state->soundHandle = -1;
        switch (D_800E2368->phase) {
        case 0:
            state->attachment = 0x20;
            state->ay -= spawn->lift;
            handle = func_800D3FD8();
            handle = func_800D3F64(0x58D, handle);
            state->soundHandle = handle;
            break;
        case 1:
            state->attachment = 0x16;
            break;
        case 2:
            state->attachment = 0x1B;
            state->ay += spawn->lift;
            break;
        }
        pool = D_800F32D0->pool;
        func_800CE8F0(pool, state->attachment, &rotation, state);
        state->y = g_RoomFloorY->y;
        func_800CFB7C(&state->ax, 0x12C, &target);
        state->x += target.x;
        state->y += target.y;
        state->z += target.z;
        if (D_800E2368->active) {
            RoomSoundBurstNode **slot = (RoomSoundBurstNode **)D_800F32D0->pool;
            if (slot && *slot) {
                u8 *flag = (*slot)->state;
                if (*flag == 1) *flag = 2;
            }
        }
        state->state = 0;
        state->frame = 0;
        pool = D_800F33E0->pool;
        return func_800CE560(pool, 24, 16, RoomEffect_HomingSpark);
    case 1:
        switch (state->state) {
        case 0:
            state->frame++;
            if ((s16)state->frame < 16) break;
            state->state = 1;
            state->frame = 0;
            break;
        case 1:
            state->frame++;
            if ((s16)state->frame < 4) break;
            state->state = 2;
            state->frame = 0;
            pool = D_800F33E0->pool;
            child = func_800CE610(pool);
            if (child) {
                child->x = state->x;
                child->y = state->y;
                child->z = state->z;
                child->wx = state->ax;
                child->wy = state->ay;
                child->wz = state->az;
                child->wx = 0;
                child->state = 0;
                child->timer = 0;
                child->soundHandle = state->soundHandle;
            }
            pool = D_800F33E0->pool;
            child = func_800CE610(pool);
            if (child) {
                child->x = state->x;
                child->y = state->y;
                child->z = state->z;
                child->state = 1;
                child->timer = 0;
            }
            break;
        case 2:
            state->frame++;
            if ((s16)state->frame < 4) break;
            return 2;
        }
        break;
    case 2:
        pool = D_800F32D0->pool;
        func_800CE8F0(pool, state->attachment, &rotation, &target);
        {
            int index;
            int tpage;
            unit = 0x20;
            index = D_800E11EC;
            D_800F3368.parameter00 = unit;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = unit;
            D_800F3368.extent_y = unit;
            tpage = D_800E2850[index];
            D_800F3368.palette = 4;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0xC;
            D_800F3368.tpage = tpage;
        }
        switch (state->state) {
        case 0:
            rsin((s16)state->frame << 6);
            opacity = (state->frame & 1) ? 0x40 : 0x80;
            scale = rsin((s16)state->frame << 6);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800CEE20(&target, 0, scale, scale, 0x20, clut, 1, opacity, 0);
            break;
        case 1:
            blend = (s16)state->frame << 10;
            LoadAverageShort12(&target, state, 0x1000 - blend, blend, &output);
            func_800CF3AC(g_RoomFlashColorRamp, &color,
                          (s16)state->frame);
            matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            func_800D2B58(&target, &output, &color, &color, 0, 0x80, 1);
            break;
        case 2:
            frame = (s16)state->frame;
            if (frame >= 5) break;
            func_800CF3AC(g_RoomFlashColorRamp, &color, frame + 4);
            matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            func_800D2B58(&target, state, &color, &color, 0, 0x80, 1);
            break;
        }
        break;
    }
    return 0;
}
