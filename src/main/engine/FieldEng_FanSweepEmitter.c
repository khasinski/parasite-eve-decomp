#include "pe1/room_ember_burst.h"
#include "pe1/gte.h"

/* Fan sweep controller: opens a spark pool and a trail pool, launches a
 * spark on a turning angle every other frame for 32 frames, updates the
 * trail for 70 frames and publishes its pool for the sparks.
 * Matching debt: four register pins and one empty slot-address barrier.
 * Matrix loads are C; each GTE transfer uses its individual macro. */
int func_800D751C(int mode, FieldFanSweep *sweep)
{
    RoomFanSweepSpark *spark;
    int handles;

    switch (mode) {
    case 0:
        sweep->angle = rand();
        handles = func_800CE560(D_800F33E0->pool, 0x10, 0x18, func_800D71B8);
        handles += func_800CE5AC(&sweep->pool, handles, 8, 0x12, func_800D70C0);
        return handles;
    case 1:
        if (D_800E27EC < 0x20 && (D_800E27EC & 1)) {
            spark = (RoomFanSweepSpark *)func_800CE610(D_800F33E0->pool);
            if (spark) {
                spark->angle = sweep->angle;
                spark->radius = 0xC8;
                spark->state = 0;
                spark->timer = 0;
                sweep->angle -= 0x555;
            }
        }
        if (D_800E27EC >= 0x46)
            return 1;
        func_800CE688(sweep->pool);
        D_800E21E8 = sweep->pool;
        break;
    case 2:
        {
            s32 **slot;
            register const GteMatrixWords *matrix asm("$8");
            register u32 a asm("$12");
            register u32 b asm("$13");
            register u32 c asm("$14");
            slot = &D_800BCFA4.value;
            asm volatile("" : "=r"(slot) : "0"(slot));
            matrix = (const GteMatrixWords *)*slot;
            a = matrix->r11_r12;
            b = matrix->r13_r21;
            gte_ctc2_0(a);
            gte_ctc2_1(b);
            a = matrix->r22_r23;
            b = matrix->r31_r32;
            c = matrix->r33_pad;
            gte_ctc2_2(a);
            gte_ctc2_3(b);
            gte_ctc2_4(c);
            a = matrix->tx;
            b = matrix->ty;
            gte_ctc2_5(a);
            c = matrix->tz;
            gte_ctc2_6(b);
            gte_ctc2_7(c);
        }
        D_800F3368.depth = 8;
        func_800CE78C(sweep->pool);
        func_800CE870((char *)D_8009D254, 1, &D_800E21E0.x);
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.tpage = D_800E2850[D_800E11F6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        break;
    }
    return 0;
}

/* Glow under the fan sweep: a pulsing sprite at the actor for 60 frames,
 * plus a flickering ring while the event is not in stage 11. */
int func_800D7764(int mode, FieldFanSweepGlow *glow)
{
    GteRotation rotation;
    GteShortVector position;
    RenderColor color = D_800C22D8;
    int intensity;
    int scale;
    int kind;
    int palette;
    int ring;

    switch (mode) {
    case 0:
        glow->state = 0;
        glow->timer = 0;
        func_800CE870((char *)D_8009D254, 0, &glow->position.x);
        break;
    case 1:
        if (D_800E27EC >= 60)
            return 1;
        break;
    case 2:
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.tpage = D_800E2850[D_800E11F6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 100;
        intensity = rsin((D_800E27EC << 11) / 60) / 32;
        kind = D_800F336C;
        palette = D_800E1204[kind];
        scale = 0x2800;
        func_800CEE20(&glow->position, 0, scale, scale, 4,
                      GetClut(0, (kind == 4 && D_800F3428) ? palette + 6 : palette + 2),
                      3, intensity, 0);
        if (D_800E2368->stage != 11) {
            position.x = glow->position.x;
            position.y = glow->position.y;
            position.z = glow->position.z;
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = D_800E27EC * 8;
            rotation.flags = 0;
            scale = (D_800E27EC & 1) * 60 + 0x1000;
            ring = D_800E2368->stage == 10 ? 1 : 3;
            func_800D0728(&position, 400, 500, 20, &rotation, scale, scale, 0,
                          &color, intensity, ring);
        }
        break;
    }
    return 0;
}
