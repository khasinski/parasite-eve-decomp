#include "pe1/room_sound_burst.h"
#ifndef ROOMEFFECT_HOMING_SPARK_FUNC
#error "ROOMEFFECT_HOMING_SPARK_FUNC must name the room entry point"
#endif

extern RoomSoundBurstColor D_8018F1CC;
extern GteRotation D_8018F1D0;
extern u16 D_800F336C;
extern s16 D_800F336A;

/* Spark that homes on the player, flags the battle actor when it arrives,
 * then burns out; sub-states 1 and 2 play the hit and fade animations. */
int ROOMEFFECT_HOMING_SPARK_FUNC(int mode, RoomHomingSpark *spark,
                                 RoomHomingSparkParams *params) {
    RoomSoundBurstColor color = D_8018F1CC;
    GteRotation rotation = D_8018F1D0;
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
            kind = D_800F336C;
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
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0, palette);
            func_800CEE20(spark, &rotation, 0x1000, 0x1000,
                          D_800F336A * spark->timer, clut, 1, 0x80, &color);
            break;
        }
        case 2: {
            int kind;
            int palette;
            fade = 0x29 - spark->timer * 41 / 16;
            kind = D_800F336C;
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

#undef ROOMEFFECT_HOMING_SPARK_FUNC
