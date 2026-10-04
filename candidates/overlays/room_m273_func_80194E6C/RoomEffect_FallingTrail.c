#include "room_m273_boss.h"
#include "pe1/gte.h"

/* Falling trail record: spins down from the boss, records where it lands or
 * touches the player, and draws a fading two-point trail ring behind it. */
int func_80194E6C(int mode, RoomM273FallingTrail *trail) {
    GteMatrix matrix;
    GteShortVector vector;
    GteShortVector point;
    GteRotation rotation;
    s16 *source;
    int i;
    s16 floor;
    s16 intensity;
    s16 scale;
    int tile;

    if (mode == 1) {
        if (trail->landed) {
            return --trail->count <= 0;
        }
        if (trail->spin) {
            trail->yaw += trail->spin;
            trail->spin += trail->spin < 0 ? -2 : 2;
        }
        vector.x = trail->pitch;
        vector.y = trail->yaw;
        vector.z = 0;
        RotMatrixYXZ(&vector, &matrix);
        for (i = 0, source = &trail->position.x; i < 3; i++) {
            matrix.t[i] = *source++;
        }
        gte_ldrotmatrix(&matrix);
        gte_ldtransmatrix(&matrix);
        gte_ldv0(&D_8019ACA4);
        gte_rtv0tr_mac();
        gte_stsv(&trail->position);
        floor = D_800942EC.value;
        if (trail->position.y >= floor - 0x80) {
            trail->position.y = floor;
            trail->landed = 1;
        }
        if (trail->position.y >= D_8019AE9C.floor && g_PlayerEntity->mode >= 4) {
            int dx = g_PlayerEntity->position[0] - trail->position.x;
            int dz = g_PlayerEntity->position[2] - trail->position.z;
            if (Math_IntSqrt(dx * dx + dz * dz) < 0x140) {
                trail->landed = 1;
                source = &trail->position.x;
                for (i = 0; i < 3; i++) {
                    D_8019AE9C.hit[i] = (g_PlayerEntity->transforms->t[i] + source[i]) / 2;
                }
                D_8019AE9C.hit_flag = 1;
                g_PlayerEntity->actor->flags |= 0x4000;
                if (D_800F32D0->instance->owner) {
                    D_800F32D0->instance->owner->flags |= 0x80000000;
                }
            }
        }
        if (trail->landed) {
            i = D_8019AE9C.landing_count++;
            D_8019AE9C.landing_x[i] = trail->position.x;
            D_8019AE9C.landing_y[i] = trail->position.y;
            D_8019AE9C.landing_z[i] = trail->position.z;
        }
        trail->head = (trail->head - 1) & 7; i = trail->head;
        gte_ldv0(&D_8019ACAC);
        gte_rtv0tr_mac();
        gte_stsv(&vector);
        trail->trail_y[i] = vector.y;
        i *= 2;
        trail->trail_x[i] = vector.x;
        trail->trail_z[i] = vector.z;
        gte_ldv0(&D_8019ACB4);
        gte_rtv0tr_mac();
        gte_stsv(&vector);
        i++;
        trail->trail_x[i] = vector.x;
        trail->trail_z[i] = vector.z;
        if (trail->count < 8) {
            trail->count++;
        }
    } else if (mode == 2) {
        if (!trail->landed) {
            int kind;
            int palette;
            int index = D_800E11EA;

            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            D_800F3368.tpage = D_800E2850[index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            rotation.x = trail->pitch + 0x400;
            rotation.y = trail->yaw;
            rotation.z = -0x400;
            rotation.flags = 1;
            if (D_800E27EC < 17) {
                scale = D_800966EC[(D_800E27EC << 8 & 0x3F00) >> 2].sine * 3 * 2048 / 4096 + 0x800;
            } else {
                scale = 0x2000;
            }
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) {
                palette += 4;
            }
            func_800CEE20(&trail->position, &rotation, scale, 0x1000, 0xC,
                          GetClut(0x30, palette), 1, 0x80, 0);
            intensity = 0x80;
        } else {
            intensity = trail->count * 16;
        }
        scale = (0x80 - intensity) * 64 + 0x1000;
        tile = D_800E11E8;
        D_800F3368.parameter02 = 1;
        D_800F3368.parameter00 = 0x10;
        D_800F3368.extent_x = 0x10;
        D_800F3368.extent_y = 0x10;
        tile = D_800E2850[tile];
        D_800F3368.palette = 2;
        D_800F3368.parameter06 = 0;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = D_800E27EC << 8;
        rotation.flags = 0;
        D_800F3368.tpage = tile;
        {
            s16 step;
            s16 index = trail->head;

            for (step = 0; step < trail->count; step++, index = (index + 1) & 7) {
                s16 side;

                point.y = trail->trail_y[index];
                for (side = 0; side < 2; side++) {
                    int kind;
                    int palette;

                    point.x = trail->trail_x[index * 2 + side];
                    point.z = trail->trail_z[index * 2 + side];
                    kind = D_800F3368.palette;
                    palette = D_800E1204[kind];
                    if (kind == 4 && D_800F3428) {
                        palette += 4;
                    }
                    func_800CEE20(&point, &rotation, scale, scale, 0xDC,
                                  GetClut(0x30, palette), 1, intensity, &D_8019ACBC);
                    rotation.y += 0x800;
                }
                intensity -= 0x10;
                scale += 0x400;
                rotation.z += 0x100;
            }
        }
    }
    return 0;
}
