#include "common.h"
#include "pe1/room_m086_seeker.h"

int func_8018F004(int mode, RoomM086Seeker *seeker, RoomM086SeekerParams *params) {
    GteShortVector rotation;
    GteShortVector position;
    GteMatrix matrix;
    GteVector scale;
    RoomM086FloorLevel *floor;
    RoomM086Channel *channel;
    u16 *index;
    int size = params->size >> 3;
    int fall;
    int bounce;
    int page;

    switch (mode) {
    case 1:
        switch (seeker->state) {
        case 0:
            seeker->timer++;
            func_800CFB7C(&seeker->heading, params->distance, &position);
            seeker->position.x += position.x;
            seeker->position.y += position.y;
            seeker->position.z += position.z;
            floor = &D_800942EC;
            if (seeker->position.y >= floor->count) {
                seeker->state = 1;
                seeker->timer = 0;
            }
            if (func_800C6B90(&seeker->position, size * 41 / 512) == 0) break;
            if (seeker->position.y < floor->count - 0x202) break;
            if (D_800E2368->active) {
                channel = D_800F32D0;
                if ((channel->pool->object->flags & 0x3F000000) == 0x01000000) {
                    D_8009D254->actor->flags |= 0x4000;
                    channel->pool->object->flags =
                        (channel->pool->object->flags & 0xC0FFFFFF) | 0x11000000;
                    channel->pool->object->flags |= 0x80000000;
                }
            }
            seeker->state = 1;
            seeker->timer = 0;
            break;
        case 1:
            seeker->timer++;
            seeker->position.y -= 6;
            if (seeker->timer >= 8) return 1;
            break;
        case 2:
            seeker->timer++;
            seeker->position.x += seeker->heading.x;
            seeker->position.y += seeker->heading.y;
            seeker->position.z += seeker->heading.z;
            seeker->heading.x = seeker->heading.x * 31 / 32;
            seeker->heading.z = seeker->heading.z * 31 / 32;
            fall = (u16)seeker->heading.y + 4;
            seeker->heading.y = fall;
            if (seeker->position.y >= D_800942EC.count) {
                bounce = -(s16)fall;
                seeker->heading.y = bounce;
            }
            if (seeker->timer >= 0x20) return 1;
            break;
        }
        break;
    case 2:
        switch (seeker->state) {
        case 0:
            rotation.x = seeker->heading.x;
            rotation.y = seeker->heading.y;
            rotation.z = seeker->heading.z;
            rotation.x += 0x800;
            rotation.z = seeker->timer * 256;
            index = &D_800E11EA;
            D_800F3368.tpage = D_800E2850[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[*index] | GetTPage(0, 0xFF, 0, 0));
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                GsSetOrign(page, func_80077AA4(0x20, palette));
            }
            func_800C6ED8(0);
            RotMatrixYXZ(&rotation, &matrix);
            matrix.t[0] = seeker->position.x;
            matrix.t[1] = seeker->position.y;
            matrix.t[2] = seeker->position.z;
            scale.x = size;
            scale.y = size;
            scale.z = size;
            Gte_ScaleMatrix(&matrix, &scale);
            func_800C6EF8(D_80190B84);
            func_800C6FA0(D_80190B84, 0x80);
            func_800C71E4(D_80190B84, &matrix);
            func_800C6F4C(D_80190B84);
            D_800F3368.parameter00 = 0x10;
            D_800F3368.parameter02 = 1;
            D_800F3368.extent_x = 0x10;
            D_800F3368.extent_y = 0x10;
            D_800F3368.tpage = D_800E2850[D_800E11E4[2]];
            D_800F3368.palette = 2;
            D_800F3368.parameter06 = 0;
            rotation.x = 0x400;
            rotation.y = 0;
            rotation.z = 0;
            rotation.pad = 1;
            position.x = seeker->position.x;
            position.y = seeker->position.y;
            position.z = seeker->position.z;
            position.y = D_800942EC.count;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20(&position, (GteRotation *)&rotation, size * 8, size * 8, 0xE8,
                              func_80077AA4(0x50, palette), 2, 0x80, 0);
            }
            break;
        case 1:
            D_800F3368.parameter00 = 0x10;
            D_800F3368.parameter02 = 1;
            D_800F3368.extent_x = 0x10;
            D_800F3368.extent_y = 0x10;
            D_800F3368.tpage = D_800E2850[D_800E11E4[2]];
            D_800F3368.palette = 2;
            D_800F3368.parameter06 = 0;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20(&seeker->position, 0, size * 8, size * 8,
                              (s16)D_800F3368.parameter02 * seeker->timer + 0xC8,
                              func_80077AA4(0x10, palette), 1, 0x80, 0);
            }
            break;
        case 2:
            func_80077DC4(seeker->timer << 5);
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = D_800E27EC.value << 7;
            rotation.pad = 0;
            D_800F3368.parameter02 = 1;
            D_800F3368.parameter00 = 0x10;
            D_800F3368.extent_x = 0x10;
            D_800F3368.extent_y = 0x10;
            D_800F3368.tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20(&seeker->position, (GteRotation *)&rotation, 0x555, 0x555, 0x1A,
                              func_80077AA4(0x10, palette), 1, 0x80, 0);
            }
            break;
        }
        break;
    }
    return 0;
}
