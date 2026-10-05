#include "common.h"
#include "pe1/room_m005_beacon.h"

/* Joint beacon: rides joint 0x14, sheds drifting sprites for the first
 * frames, swells and shrinks, tags the player on contact; draws four passes
 * of a model, three glow sprites and a turning second model. */
int func_8018FDC4(int mode, RoomM005Beacon *state) {
    GteVector scale;
    GteShortVector position;
    GteShortVector rotation;
    RoomM005BeaconOffset offset = D_8018F010;
    GteShortVector spinAngles;
    GteMatrix matrix;
    GteMatrix spin;
    GteVector spinScale;
    RoomM005BeaconSprite *sprite;
    int size;
    int alpha;
    int page;

    switch (mode) {
    case 0:
        func_800CE8F0(D_800F32D0->pool, 0x14, &offset, state);
        func_800CE9D4(D_800F32D0->pool, 0, &state->rotation);
        state->rotation.pad = 1;
        state->phase = 0;
        state->timer = 0;
        state->height = 0;
        if (D_800E2368->active) {
            RoomM005BeaconPool *pool = D_800F32D0->pool;
            if (pool && pool->node) {
                u8 *flag = pool->node->state;
                if (*flag == 1) *flag = 2;
            }
        }
        D_80190B94 = func_8006E498(D_800B0E64.channel, 0xC5462704);
        func_800C6D5C(D_80190B94, 0, 0);
        D_80190B98 = func_8006E498(D_800B0E64.channel, 0xC5862704);
        func_800C6D5C(D_80190B98, 0, 0);
        return func_800CE560(D_800F33E0->pool, 0x10, 8, func_8018FB84);
    case 1:
        if (D_800E27EC < 10) {
            sprite = func_800CE610(D_800F33E0->pool);
            if (sprite) {
                sprite->x = (func_80071A54() & 0x3FF) - 0x200;
                sprite->y = (func_80071A54() & 0x3FF) - 0x200;
                sprite->z = 0;
                sprite->vx = (func_80071A54() & 7) - 3;
                sprite->vy = (func_80071A54() & 7) - 3;
                sprite->vz = 0;
            }
        }
        switch (state->phase) {
        case 0:
            state->timer++;
            state->width = 0x1000;
            state->height += 0x200;
            if (state->timer >= 5) {
                state->phase = 1;
                state->timer = 0;
            }
            break;
        case 1:
            state->timer++;
            state->width = 0x1000 - (state->timer << 9);
            if (state->timer >= 8) return 1;
            break;
        }
        func_800CFB7C(&state->rotation, state->height, &position);
        state->target.x = state->position.x;
        state->target.y = state->position.y;
        state->target.z = state->position.z;
        state->target.x += position.x;
        state->target.y += position.y;
        state->target.z += position.z;
        if (func_800CEB8C(state, &state->target, 100) == 0) return 0;
        if (D_800E2368->active) {
            RoomM005BeaconChannel *channel = D_800F32D0;
            if ((channel->pool->node->flags & 0x3F000000) != 0x01000000) return 0;
            D_8009D254->actor->flags |= 0x4000;
            channel->pool->node->flags =
                (channel->pool->node->flags & 0xC0FFFFFF) | 0x11000000;
            channel->pool->node->flags |= 0x80000000;
        }
        break;
    case 2:
        D_800F3368.depth = 8;
        matrix = *D_800F32D0->pool->matrix;
        scale.x = scale.y = state->width / 40;
        scale.z = (state->height << 12) / 8680;
        Gte_ScaleMatrix(&matrix, &scale);
        matrix.t[0] = state->position.x;
        matrix.t[1] = state->position.y;
        matrix.t[2] = state->position.z;
        {
            RoomM005BeaconMatrixSlot *slot;
            u16 *index = &D_800E11EA;
            u16 *tpages = D_800E2850;
            D_800F3368.tpage = D_800E2850[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            slot = &D_800BCFA4;
            gte_ldrotmatrix(slot->value);
            gte_ldtransmatrix(slot->value);
            page = (u16)(tpages[*index] | GetTPage(0, 1, 0, 0));
        }
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            GsSetOrign(page, GetClut(0x30, palette));
        }
        func_800C6ED8(1);
        func_800C6EF8(D_80190B94);
        func_800C6FA0(D_80190B94, 0x32);
        func_800C71E4(D_80190B94, &matrix);
        func_800C6F4C(D_80190B94);
        func_800C6EF8(D_80190B94);
        scale.z = 0x1000;
        scale.x = scale.y = 0x2000;
        Gte_ScaleMatrix(&matrix, &scale);
        func_800C70EC(D_80190B94, -0xFF, -0x82, 0x30);
        func_800C71E4(D_80190B94, &matrix);
        func_800C6F4C(D_80190B94);
        func_800C6EF8(D_80190B94);
        scale.x = scale.y = 0x400;
        Gte_ScaleMatrix(&matrix, &scale);
        func_800C6FA0(D_80190B94, 0xC8);
        func_800C71E4(D_80190B94, &matrix);
        func_800C6F4C(D_80190B94);
        func_800C6EF8(D_80190B94);
        matrix.t[1] = D_800942EC;
        scale.x = scale.y = 0x2000;
        Gte_ScaleMatrix(&matrix, &scale);
        func_800C6FA0(D_80190B94, 0x18);
        func_800C71E4(D_80190B94, &matrix);
        func_800C6F4C(D_80190B94);
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.tpage = D_800E2850[D_800E11E4[11]];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0x20;
        alpha = 0xA0;
        size = state->width;
        if (D_800E27EC & 1) {
            alpha = 0x80;
            size = size * 3 / 2;
        }
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            func_800CEE20(&state->position, 0, size, size, 2,
                          GetClut(0, (kind == 4 && D_800F3428) ? palette + 8 : palette + 4),
                          1, alpha, 0);
        }
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            func_800CEE20(&state->target, 0, state->width / 2, state->width / 2, 2,
                          GetClut(0, (kind == 4 && D_800F3428) ? palette + 8 : palette + 4),
                          1, alpha, 0);
        }
        position.x = state->position.x;
        position.y = state->position.y;
        position.z = state->position.z;
        position.y = D_800942EC;
        rotation.y = 0;
        rotation.z = 0;
        rotation.x = 0x400;
        rotation.pad = 1;
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            func_800CEE20(&position, &rotation, size, size, 2,
                          GetClut(0, (kind == 4 && D_800F3428) ? palette + 8 : palette + 4),
                          3, alpha, 0);
        }
        if (D_800E27EC < 0xD) {
            alpha = func_80077DC4((D_800E27EC << 10) / 12) / 32;
            size = func_80077CF4((D_800E27EC << 10) / 12);
            {
                RoomM005BeaconMatrixSlot *slot = &D_800BCFA4;
                gte_ldrotmatrix(slot->value);
                gte_ldtransmatrix(slot->value);
            }
            {
                u16 *index = &D_800E11FA;
                D_800F3368.tpage = D_800E2850[*index];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                page = (u16)(D_800E2850[*index] | GetTPage(0, 1, 0, 0));
            }
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                GsSetOrign(page, GetClut(0, (kind == 4 && D_800F3428) ? palette + 7
                                                                     : palette + 3));
            }
            func_800C6ED8(1);
            spinAngles.x = state->rotation.x;
            spinAngles.y = state->rotation.y;
            spinAngles.z = state->rotation.z;
            spinAngles.x += 0x800;
            spinAngles.z = D_800E27EC << 7;
            RotMatrixYXZ(&spinAngles, &spin);
            spin.t[0] = state->position.x;
            spin.t[1] = state->position.y;
            spin.t[2] = state->position.z;
            spinScale.z = spinScale.y = spinScale.x = size / 2;
            Gte_ScaleMatrix(&spin, &spinScale);
            func_800C6EF8(D_80190B98);
            func_800C6FA0(D_80190B98, alpha / 2);
            func_800C71E4(D_80190B98, &spin);
            func_800C6F4C(D_80190B98);
        }
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.tpage = D_800E2850[D_800E11EA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 4;
        D_80190BA8.x = state->position.x;
        D_80190BA8.y = state->position.y;
        D_80190BA8.z = state->position.z;
        D_80190BA0.x = state->rotation.x;
        D_80190BA0.y = state->rotation.y;
        D_80190BA0.z = 0;
        D_80190BA0.x = 0;
        break;
    }
    return 0;
}
