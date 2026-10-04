#include "room_m273_boss.h"

/* Sway shard: flies until it hits the player or the floor (recording the
 * impact in the sway point table), leaving a trail; draws a glow, two
 * crossed flares and two floor rings. */
int func_801981A4(int mode, RoomM273SwayShard *shard) {
    GteShortVector position;
    GteRotation spin;

    if (mode == 1) {
        if (shard->state == 0 && D_8019AF74.cooldown == 0 &&
            shard->position.y > (s16)D_800942EC.value - 0x240) {
            s16 dx = g_PlayerEntity->position[0] - shard->position.x;
            s16 dz = g_PlayerEntity->position[2] - shard->position.z;
            if ((s16)Math_IntSqrt(dx * dx + dz * dz) < 0x80) {
                shard->state = 1;
                g_PlayerEntity->actor->flags |= 0x4000;
                if (D_800F32D0->instance->owner)
                    D_800F32D0->instance->owner->flags |= 0x80000000;
                D_8019AF74.hits[shard->side].x = shard->position.x;
                D_8019AF74.hits[shard->side].y = shard->position.y;
                D_8019AF74.hits[shard->side].z = shard->position.z;
                D_8019AF74.hits[shard->side].pad = 2;
                Asset_Find08w(0x5FC, D_800F32D0->instance->owner->sound, shard->position.x,
                              shard->position.y, shard->position.z);
                D_8019AF74.cooldown = 0x28;
                return 1;
            }
        }
        shard->position.x += shard->velocity.x;
        shard->position.y += shard->velocity.y;
        shard->position.z += shard->velocity.z;
        if (shard->position.y >= (s16)D_800942EC.value) {
            D_8019AF74.hits[shard->side].x = shard->position.x;
            D_8019AF74.hits[shard->side].y = shard->position.y;
            D_8019AF74.hits[shard->side].z = shard->position.z;
            D_8019AF74.hits[shard->side].pad = 3;
            Asset_Find08w(0x5FC, D_800F32D0->instance->owner->sound, shard->position.x,
                          shard->position.y, shard->position.z);
            return 1;
        } else {
            s16 frame = D_800E27EC & 7;
            GteShortVector *trail;
            if (frame < 4) shard->frame = frame;
            else shard->frame = frame + 12;
            trail = func_800CE610(D_8019AF70);
            if (trail == 0) return 0;
            trail->x = shard->position.x;
            trail->y = shard->position.y;
            trail->z = shard->position.z;
        }
    } else if (mode == 2) {
        int i;
        int kind;
        int palette;
        position.x = shard->position.x;
        position.y = shard->position.y;
        position.z = shard->position.z;
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;
        func_800CEE20(&position, 0, 0xA00, 0xA00,
                      (s16)D_800F3368.parameter02 * shard->frame,
                      GetClut(0x10, palette), 1, 0x80, 0);
        spin.x = D_800E27EC * 0x300;
        spin.y = D_800F32D0->instance->yaw;
        spin.z = 0;
        spin.flags = 1;
        for (i = 0; i < 2; i++) {
            func_800D0728(&position, 0x60, 0x88, 8, &spin, 0x1000, 0x1000, &D_8019AE04,
                          D_8019AB70, 0x80, 1);
            spin.y = spin.x;
            spin.x = 0;
        }
        spin.x = 0;
        spin.y = 0;
        spin.flags = 0;
        spin.z = D_800E27EC << 8;
        func_800D004C(&position, 0x100, 0x100, 8, &spin, 0x1000, 0x1000, &D_8019AE08,
                      (RenderColor *)D_8019AB70, 0x80, 1);
        position.y = D_800942EC.value;
        func_800D004C(&position, 0xC0, 0xC0, 8, 0, 0x1000, 0x1000, &D_8019AE0C,
                      (RenderColor *)D_8019AB70, 0x80, 1);
    }
    return 0;
}
