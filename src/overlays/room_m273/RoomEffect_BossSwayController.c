#include "room_m273_boss.h"
#include "pe1/gte.h"

/* Animation 15 controller: transforms two model points, then sways the
 * boss toward or away from the player for eight frames. */
int func_80199C50(int mode) {
    RoomM273BossInstance *instance = D_800F32D0->instance;
    GteShortVector offset;
    int value; /* loop index, then the player heading */
    int frame;
    int frame_1A;
    GteShortVector *point;

    if (mode == 0) {
        D_8019AF74.done = 0;
        instance->owner->flags |= 0x40000000;
        D_8019AF74.points[0].pad = 0;
        D_8019AF74.points[1].pad = 1;
        D_8019AF74.points[2].pad = 0;
        D_8019AF74.points[3].pad = 0;
        D_8019AF74.sway_timer = 0;
        D_8019AF74.cooldown = 0;
    } else if (mode == 1) {
        if (*instance->owner->status == 1) *instance->owner->status = 2;
        frame = instance->time.parts.frame;
        frame_1A = instance->frame_1A;
        D_8019AF74.animation = instance->animation;
        D_8019AF74.frame = frame;
        D_8019AF74.frame_1A = frame_1A;
        offset.x = 0;
        offset.y = 0;
        offset.z = -0x130;
        point = D_8019AF74.points;
        for (value = 0; value < 2; value++) {
            GteMatrix *m = instance->transforms;
            if (value) m += 36;
            else m += 25;
            gte_ldrotmatrix(m);
            gte_ldtransmatrix(m);
            gte_ldv0(&offset);
            gte_rtv0tr_mac();
            gte_stsv(&point[value]);
        }
        if (instance->animation == 15) {
            if ((s16)frame >= instance->frame_count - 1) {
                D_8019AF74.done = 1;
                if (instance->owner) *instance->owner->status = 4;
                return 1;
            }
            if ((s16)frame >= 0x16) {
                if (D_8019AF74.repeat-- > 0) instance->time.fixed = 0;
            }
        }
        if (D_8019AF74.repeat && (s16)frame >= 0xF && (s16)frame_1A < 0xF) {
            int step;
            D_8019AF74.sway_timer = 8;
            value = (s16)FieldEng_VecToAngle(g_PlayerEntity->position, instance->position);
            step = 0x20;
            if ((value - instance->yaw) & 0x800) step = -0x20;
            D_8019AF74.sway_step = step;
        }
        if (D_8019AF74.sway_timer) {
            instance->yaw += D_8019AF74.sway_step *
                             D_800966EC[(D_8019AF74.sway_timer << 8) & 0x1F00] / 4096;
            D_8019AF74.sway_timer--;
        }
        if (D_8019AF74.cooldown) D_8019AF74.cooldown--;
    }
    return 0;
}
