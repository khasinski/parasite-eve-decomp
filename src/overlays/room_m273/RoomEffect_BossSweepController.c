/* MASPSX_FLAGS: --expand-div */
#include "room_m273_boss.h"
#include "pe1/gte.h"

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
