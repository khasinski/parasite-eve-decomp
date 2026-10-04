#include "../../../src/overlays/room_m273/room_m273_boss.h"
#include "pe1/render_object.h"

/* Falling drop: queues its landing position, then knocks the player back on
 * contact; draws two rings before landing and a splash flash after. */
int func_8019665C(int mode, RoomM273Drop *drop) {
    GteShortVector ring;

    if (mode == 1) {
        if (drop->position.pad == 0) {
            RoomM273BossFloor *floor = &D_800942EC;
            drop->position.x += drop->vx;
            drop->position.y += drop->vy;
            drop->position.z += drop->vz;
            if (drop->position.y >= (s16)floor->value) {
                s16 count;
                drop->position.y = floor->value;
                drop->position.pad = 1;
                count = D_8019AF04.count++;
                D_8019AF04.x[count] = drop->position.x;
                D_8019AF04.y[count] = floor->value;
                D_8019AF04.z[count] = drop->position.z;
            }
            if (drop->touched) return 0;
            if (drop->position.y < D_8019AF62) return 0;
            if (Math_IntSqrt((g_PlayerEntity->position[0] - drop->position.x) *
                                 (g_PlayerEntity->position[0] - drop->position.x) +
                             (g_PlayerEntity->position[2] - drop->position.z) *
                                 (g_PlayerEntity->position[2] - drop->position.z)) < 0x80) {
                drop->touched = 1;
                D_8019AF04.hit = drop->position;
                D_8019AF04.hit.pad = 1;
                g_PlayerEntity->actor->flags |= 0x4000;
                if (D_800F32D0->instance->owner)
                    D_800F32D0->instance->owner->flags |= 0x80000000;
            }
        } else {
            if (drop->position.pad++ >= 8) return 1;
        }
    } else if (mode == 2) {
        int shade = D_800966EC[(drop->position.pad << 7) & 0xF80].cosine >> 5;
        RoomM273BossTrig *trig = D_800966EC;
        ring.x = drop->ring.x;
        ring.y = drop->ring.y;
        ring.z = drop->ring.z;
        if (drop->position.pad == 0) {
            int i;
            for (i = 0; i < 2; i++) {
                func_800D0E88(drop, &ring, 0x100, 0x10, D_8019AD58, D_8019AD54,
                              D_8019AD54, shade, 1);
                ring.z += 0x400;
            }
        } else {
            int size = trig[(drop->position.pad << 7) & 0xF80].sine * 2 + 0x1000;
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            func_800CEE20(&drop->position, (GteRotation *)&D_8019AB68, size, size, 0xDC,
                          GetClut(0x30, palette), 1, shade, (RenderColor *)D_8019AD5C);
        }
    }
    return 0;
}
