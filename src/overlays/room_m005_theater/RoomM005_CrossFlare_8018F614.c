#include "common.h"
#include "pe1/room_m005.h"
#include "pe1/room_sound_slot.h"

/* Cross flare: two small sprites either side of joint 9 while the frame
 * counter is below 35, then a flash track, two streaks and a large turning
 * sprite at joint 20. The first draw's intensity, the streak step and its
 * width are separate variables (all land in s0), while the streak fade and
 * the final size share one, and parameter02 is written second: each of
 * these decides retail's register choice. */

int func_8018F614(int mode) {
    GteShortVector position;
    GteRotation rotation;
    RoomM005Seed8 outer = D_8018EFFC;
    RoomM005Seed8 inner = D_8018F004;
    RenderColor color;
    RoomSoundSlot *soundSlot;
    int volume;
    int handle;
    int size;
    int step;
    int width;
    int level;

    switch (mode) {
    case 0:
        soundSlot = &D_800B0E64_slot;
        if (soundSlot->channel != 0) {
            volume = 0x7F;
            handle = func_800D3FD8();
            func_8006DF50(soundSlot->channel, 0x5F8, handle, 0x80, volume);
            if (soundSlot->channel != 0) {
                func_8006DF50(soundSlot->channel, 0x5F9, 0x80, 0x80, volume);
            }
        }
        break;
    case 1:
        if (D_800E27EC >= 35) return 1;
        break;
    case 2:
        D_800F3368.depth = 0x40;
        func_800CE8F0(D_800F32D0->pool, 9, &inner, &position);
        if (D_800E27EC < 35) {
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            D_800F3368.tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = D_800E27EC << 6;
            rotation.flags = 0;
            level = func_80077CF4((D_800E27EC << 11) / 34);
            position.x -= 0x10;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20(&position, &rotation, 0x1000, 0x1000, 2,
                              (u16)func_80077AA4(0x10, palette), 1, level, 0);
            }
            position.x += 0x20;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20(&position, &rotation, 0x1000, 0x1000, 2,
                              (u16)func_80077AA4(0x10, palette), 1, level, 0);
            }
        }
        D_800F3368.depth = 0x18;
        func_800CE8F0(D_800F32D0->pool, 0x14, &outer, &position);
        if (D_800E27EC < 35) {
            step = D_800E27EC - 18;
            if ((unsigned int)step < 17) {
                size = func_80077DC4(step << 6);
                width = 600 - step * 100;
                func_800CF3AC(D_80190AF4, &color, step * 2);
                func_800D0728(&position, width, 700, 0x18, 0, size, size,
                              0, &color, 0x40, 1);
                func_800D0728(&position, 700, 800, 0x18, 0, size, size,
                              &color, 0, 0x40, 1);
            }
            func_800CF3AC(D_80190AF4, &color, D_800E27EC);
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            D_800F3368.tpage = D_800E2850[D_800E11FA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 0;
            size = func_80077CF4((D_800E27EC << 10) / 34);
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = D_800E27EC << 6;
            rotation.flags = 0;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                func_800CEE20(&position, &rotation, size, size, 0x40,
                              (u16)func_80077AA4(0, (kind == 4 && D_800F3428) ? palette + 6 : palette + 2),
                              1, 0x60, &color);
            }
        }
        break;
    }
    return 0;
}
