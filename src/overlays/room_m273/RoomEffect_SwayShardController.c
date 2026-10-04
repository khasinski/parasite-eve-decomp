#include "room_m273_boss.h"
#include "pe1/render_object.h"

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
