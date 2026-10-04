#include "pe1/room_m350_drop.h"

/* Falling drop: moves until it passes the floor (splash) or touches the
 * player, otherwise queues its position for the trail; draws a glow and a
 * floor ring. */
int func_80196F2C(int mode, RoomM350Drop *drop) {
    GteShortVector position;

    if (mode == 1) {
        if (drop->landed) return 1;
        drop->x += drop->velocity.x;
        drop->y += drop->velocity.y;
        drop->z += drop->velocity.z;
        if (drop->y > D_800942EC.value) {
            g_RoomEffectTrailPositions.splashed = 1;
            g_RoomEffectTrailPositions.splash_x = drop->x;
            g_RoomEffectTrailPositions.splash_z = drop->z;
            g_RoomEffectTrailPositions.touched = 1;
            g_RoomEffectTrailPositions.hit_x = drop->x;
            g_RoomEffectTrailPositions.hit_y = drop->y - 0xA0;
            g_RoomEffectTrailPositions.hit_z = drop->z;
            func_8019721C((short *)drop, 200);
            return 1;
        }
        if (func_8019721C((short *)drop, 0x50)) {
            g_RoomEffectTrailPositions.touched = 1;
            g_RoomEffectTrailPositions.hit_x = drop->x;
            g_RoomEffectTrailPositions.hit_y = drop->y;
            g_RoomEffectTrailPositions.hit_z = drop->z;
            drop->landed = 1;
        } else if (g_RoomEffectTrailPositions.count < 4) {
            GteShortVector *point =
                &g_RoomEffectTrailPositions.points[g_RoomEffectTrailPositions.count++];
            point->x = drop->x;
            point->y = drop->y;
            point->z = drop->z;
        }
    } else if (mode == 2) {
        int kind;
        int palette;
        position.x = drop->x;
        position.y = drop->y;
        position.z = drop->z;
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11E4[11]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0;
            D_800F3368.tpage = tpage;
        }
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        func_800CEE20(&position, 0, 0xA00, 0xA00,
                      (s16)D_800F3368.parameter02 * 6 + 0x80,
                      GetClut(0, (kind == 4 && D_800F3428) ? palette + 8 : palette + 4),
                      1, 0x80, &D_8019A61C[0]);
        position.y = D_800942EC.value;
        func_800D004C(&position, 0x60, 0x60, 6, &D_8019A3C0, 0x1000, 0x1000,
                      &D_8019A61C[1], &D_8019A61C[2], 0x80, 1);
    }
    return 0;
}
