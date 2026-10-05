/* MASPSX_FLAGS: --expand-div */
#include "room_m273_boss.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"

extern char D_8019AE00[];

/* Animation 13 controller: turns the boss toward the player on entry, loops
 * the sweep window while loops remain, publishes the sweep attack angle and
 * transforms the sweep point. */
int func_80197BBC(int mode) {
    RoomM273BossInstance *instance = D_800F32D0->instance;
    RoomM273SweepStep *step;
    GteShortVector offset;
    s16 frame;
    s16 angle;
    s16 value;

    if (mode == 0) {
        D_8019AF04.started = 0;
        D_8019AF04.done = 0;
        D_8019AF04.in_window = 0;
        D_8019AF04.in_contact = 0;
        D_8019AF04.step = 0;
        D_8019AF04.count = 0;
        D_8019AF04.hit.pad = 0;
        D_8019AF04.floor = D_800942EC.value - 0x180;
        instance->owner->flags |= 0x40000000;
    } else if (mode == 1) {
        if (*instance->owner->status == 1) *instance->owner->status = 2;
        if (instance->animation == 13) {
            frame = instance->time.bits.frame;
            step = &D_8019AD80[D_8019AF04.side];
            if (frame == 0) {
                instance->time.fixed = step->start << 16;
                angle = (func_80079FB4(instance->x - g_PlayerEntity->location[0].value,
                                       instance->z - g_PlayerEntity->location[2].value)
                         - instance->yaw) & 0xFFF;
                if (D_8019AF04.side != 0) {
                    if (angle <= 0x800) D_8019AF04.turn = angle >> 6;
                    else D_8019AF04.turn = 0;
                } else {
                    angle = 0x1000 - angle;
                    if (angle <= 0x800) D_8019AF04.turn = -angle >> 6;
                    else D_8019AF04.turn = 0;
                }
            } else {
                if (frame >= step->end) {
                    D_8019AF04.done = 1;
                    if (instance->owner) *instance->owner->status = 4;
                    return 1;
                }
                if (frame >= step->loop_end && D_8019AF04.loops > 0) {
                    instance->time.fixed = step->loop_start << 16;
                    D_8019AF04.loops--;
                }
            }
            if (frame >= step->loop_start) D_8019AF04.started = 1;
            D_8019AF04.in_window = frame >= step->loop_start && frame <= step->loop_end;
            {
                RoomM273BossInstance *current = D_800F32D0->instance;
                s16 previous;
                frame = current->time.bits.frame;
                previous = current->previous.bits.frame;
                D_8019AF04.in_contact = frame >= D_8019AF04.contact && previous < D_8019AF04.contact;
            }
            if (frame >= step->loop_start && frame <= step->loop_end) {
                value = step->scale * D_8019AF04.step / D_8019AF04.divisor;
                instance->attack_kind = 0x15;
                instance->attack_power = 2;
                instance->attack_angle = value;
                D_8019AF04.target = step->offset + (value + instance->yaw);
                D_8019AF04.step++;
            } else {
                instance->attack_kind = 0;
                instance->attack_power = 0;
            }
            if (frame > step->loop_end) D_8019AF04.turn >>= 1;
            instance->yaw += D_8019AF04.turn;
        }
    }
    offset.x = 0;
    offset.y = 0;
    offset.z = -0x180;
    {
        GteMatrix *matrix = instance->transforms + 22;
        GteShortVector *out;
        gte_ldrotmatrix(matrix);
        gte_ldtransmatrix(matrix);
        gte_ldv0(&offset);
        gte_rtv0tr_mac();
        out = &D_8019AEFC;
        gte_stsv(out);
    }
    return 0;
}

int func_80198060(int mode, GteShortVector *position) {
    int frame, sample, size, kind, palette;
    u16 clut;

    if (mode == 1) {
        if (D_800E27EC >= 9) return 1;
    } else if (mode == 2) {
        frame = D_800E27EC;
        if (frame - 2 < 0) return 0;

        sample = *(s16 *)((char *)D_800966EC + (((frame - 1) << 9) & 0x3E00) + 2);
        size = sample * 2;
        kind = D_800F336C;
        /* Keep the table read before the palette lookup. */
        asm volatile("" ::: "memory");
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;

        clut = GetClut(32, palette);
        func_800CEE20(position, 0, (s16)size, (s16)size,
                       D_800F336A + 216, clut, 1,
                       /* The upper half of each packed trig entry is signed. */
                       (s16)(*(s32 *)((char *)D_800966EC +
                           (((D_800E27EC - 1) << 9) & 0x3E00)) >> 16) >> 5,
                       (RenderColor *)D_8019AE00);
    }
    return 0;
}

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

/* Spawns a shard from each sway point aimed at the player while animation
 * 15 swings through, and draws the shard and glow pools. */
int func_801986E8(int mode) {
    GteShortVector target;
    GteShortVector angles;
    GteMatrix matrix;
    int count;

    switch (mode) {
    case 0:
        count = func_800CE560(D_800F33E0->pool, 0x14, 4, func_801981A4);
        count += func_800CE5AC(&D_8019AF70, count, 8, 0x14, func_80198060);
        return count;
    case 1:
        if (D_8019AF74.done) return 2;
        if (D_800F32D0->instance->animation == 15 && D_8019AF74.frame >= 4 &&
            D_8019AF74.frame_1A < 4) {
            int i;
            for (i = 0; i < 2; i++) {
                RoomM273SwayShard *shard = func_800CE610(D_800F33E0->pool);
                RoomM273BossPlayer *player;
                int dx;
                int dy;
                int dz;
                if (shard == 0) break;
                shard->side = i;
                shard->state = 0;
                player = g_PlayerEntity;
                shard->position.x = D_8019AF74.points[i].x;
                shard->position.y = D_8019AF74.points[i].y;
                shard->position.z = D_8019AF74.points[i].z;
                target.x = player->location[0].parts.integer;
                target.y = player->location[1].parts.integer;
                target.z = player->location[2].parts.integer;
                dx = target.x - shard->position.x;
                dz = target.z - shard->position.z;
                dy = target.y - shard->position.y;
                angles.x = (Gte_Atan2(dy, Math_IntSqrt(dx * dx + dz * dz)) & 0xFFF) - 0x30;
                angles.y = D_800F32D0->instance->yaw;
                angles.z = 0;
                if (angles.x < 0x100) angles.x = 0x100;
                if (angles.x > 0x280) angles.x = 0x280;
                RotMatrixYXZ(&angles, &matrix);
                ApplyMatrixSV(&matrix, &D_8019AE10, &shard->velocity);
            }
            target.x = (D_8019AF74.points[0].x + D_8019AF74.points[1].x) / 2;
            target.y = (D_8019AF74.points[0].y + D_8019AF74.points[1].y) / 2;
            target.z = (D_8019AF74.points[0].z + D_8019AF74.points[1].z) / 2;
            Asset_Find08w(0x5D2, D_800F32D0->instance->owner->sound, target.x, target.y,
                          target.z);
        }
        func_800CE688(D_8019AF70);
        break;
    case 2:
        D_800F3368.parameter00 = 0x10;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 0x10;
        D_800F3368.extent_y = 0x10;
        D_800F3368.extent_x = 0x10;
        D_800F3368.extent_y = 0x10;
        {
            int tpage = D_800E2850[D_800E11E4[2]];
            D_800F3368.palette = 2;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0;
            D_800F3368.tpage = tpage;
        }
        func_800CE78C(D_8019AF70);
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11E4[3]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}
