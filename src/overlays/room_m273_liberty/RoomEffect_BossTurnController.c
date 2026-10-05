#include "room_m273_boss.h"

/* Tracks the boss's two hand positions, turns it toward the player (or by
 * a scripted spin) during animation 11 and bobs its height. */
int func_80196348(int mode) {
    RoomM273BossInstance *instance = D_800F32D0->instance;
    int frame;
    int value;
    GteShortVector *hand;

    if (mode == 0) {
        D_8019AE9C.done = 0;
        D_8019AE9C.landing_count = 0;
        D_8019AE9C.hit_flag = 0;
        D_8019AE9C.spin = 0;
        D_8019AE9C.tick = 0;
        D_8019AE9C.floor = D_800942EC.value - 0x200;
        D_8019AE9C.base_height = instance->height;
    } else if (mode == 1) {
        hand = D_8019AE9C.hands;
        for (value = 0; value < 2; value++) {
            GteMatrix *m = instance->transforms;
            s32 *t = value ? m[18].t : m[13].t;
            hand[value].x = t[0];
            hand[value].y = t[1];
            hand[value].z = t[2];
        }
        if (*instance->owner->status == 1) *instance->owner->status = 2;
        frame = instance->time.parts.frame;
        if (instance->animation != 11) return 0;
        if ((s16)frame >= instance->frame_count - 1) {
            D_8019AE9C.done = 1;
            if (instance->owner) *instance->owner->status = 4;
            return 1;
        }
        if ((s16)frame >= 0x22) {
            if (D_8019AE9C.repeat-- > 0) instance->time.fixed = 0x40000;
        }
        if (D_8019AE9C.spinning) {
            if (D_8019AE9C.spin > D_8019AE9C.spin_target) D_8019AE9C.spin -= D_8019AE9C.spin_step;
            else if (D_8019AE9C.spin < D_8019AE9C.spin_target) D_8019AE9C.spin += D_8019AE9C.spin_step;
            instance->yaw = (instance->yaw + D_8019AE9C.spin) & 0xFFF;
            if (D_8019AE9C.tick >= D_8019AE9C.tick_limit) D_8019AE9C.spin_target = 0;
        } else {
            /* The heading is turned half way round in 16.16 form. */
            value = FieldEng_VecToAngle(g_PlayerEntity->position, instance->position);
            instance->yaw = FieldEng_TurnToward(instance->yaw, (value << 16) + 0x8000000 >> 16, 0x20);
        }
        if (D_8019AE9C.tick < 0x77) {
            value = (D_8019AE9C.tick << 13) / 118;
            value = D_800966EC[(value - 0x800) & 0xFFF].cosine + 0x1000;
            instance->height = ((-(value * 384) / 8192) << 16) + D_8019AE9C.base_height;
        }
        D_8019AE9C.tick++;
    }
    return 0;
}
