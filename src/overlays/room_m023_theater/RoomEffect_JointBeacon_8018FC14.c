#include "common.h"
#include "pe1/room_m023_beacon.h"

/* Joint beacon: rides a model joint, swells in, pulses, flickers and
 * collapses, tagging the player once on contact; draws a glow sprite pair
 * with two star fans and a loaded model scaled by the beacon size. */
int func_8018FC14(int mode, RoomM023Beacon *state) {
    GteVector scale;
    GteShortVector position;
    GteShortVector rotation;
    RoomM023BeaconOffset offset = D_8018EFF4;
    GteMatrix matrix;
    GteMatrix spin;
    RoomM023BeaconColor color;
    int size;
    int page;

    switch (mode) {
    case 0:
        switch (D_800E2368->variant) {
        case 0:
            state->joint = 8;
            break;
        case 1:
            state->joint = 0x10;
            break;
        }
        state->phase = 0;
        state->timer = 0;
        state->height = 0;
        state->glow = 0;
        if (D_800E2368->active) {
            RoomM023BeaconPool *pool = D_800F32D0->pool;
            if (pool && pool->node) {
                u8 *flag = pool->node->state;
                if (*flag == 1) *flag = 2;
            }
        }
        state->armed = 1;
        D_80190760 = func_8006E498(D_800B0E64.channel, 0xC5463704);
        func_800C6D5C(D_80190760, 0, 0);
        return 0;
    case 1:
        func_800CE8F0(D_800F32D0->pool, state->joint, &offset, state);
        func_800CE9D4(D_800F32D0->pool, 0, &state->rotation);
        switch (state->phase) {
        case 0:
            state->timer++;
            state->width = 0x400;
            state->height += 0x19A;
            state->glow = 0x1000;
            state->shade = state->timer * 8 / 5;
            if (state->timer >= 5) {
                state->phase = 1;
                state->timer = 0;
            }
            break;
        case 1:
            state->timer++;
            state->width = func_80077DC4(state->timer << 7) + 0x1000;
            state->glow = func_80077DC4(state->timer << 7) + 0x1000;
            state->shade = state->timer + 8;
            if (state->timer >= 8) {
                state->phase = 2;
                state->timer = 0;
            }
            break;
        case 2:
            state->timer++;
            state->glow = 0x1000;
            state->shade = 0x10;
            state->width = ((D_800E27EC & 1) << 8) + 0x1000;
            if (state->timer >= 0x20) {
                state->phase = 3;
                state->timer = 0;
            }
            break;
        case 3:
            state->glow = 0x1000;
            state->timer++;
            state->width = 0x1000 - (state->timer << 8);
            state->shade = state->timer + 0x10;
            if (state->timer >= 0x10) return 1;
            break;
        }
        func_800CFB7C(&state->rotation, state->height, &position);
        state->target.x = state->position.x;
        state->target.y = state->position.y;
        state->target.z = state->position.z;
        state->target.x += position.x;
        state->target.y += position.y;
        state->target.z += position.z;
        if (func_800CEB8C(state, &state->target, 0x50) == 0) return 0;
        if (state->armed == 0) return 0;
        if (D_800E2368->active) {
            RoomM023BeaconChannel *channel = D_800F32D0;
            if ((channel->pool->node->flags & 0x3F000000) == 0x01000000) {
                D_8009D254->actor->flags |= 0x4000;
                channel->pool->node->flags =
                    (channel->pool->node->flags & 0xC0FFFFFF) | 0x11000000;
                channel->pool->node->flags |= 0x80000000;
            }
        }
        state->armed = 0;
        break;
    case 2:
        D_800F3368.depth = 0x18;
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        D_800F3368.tpage = D_800E2850[D_800E11EA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        if (state->glow != 0) {
            rotation.x = 0;
            rotation.y = 0;
            rotation.pad = 0;
            rotation.z = D_800E27EC * 0x60;
            size = state->glow;
            size = size * 2 / 3;
            if (D_800E27EC & 1) size = size * 9 / 8;
            func_800CF3AC(D_801906F0, &color, state->shade);
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20(&state->position, &rotation, size, size, 0x80,
                              GetClut(0x30, palette), 1, 0x80, &color);
            }
            func_800D004C(&state->position, 0x190, 0x190, 0xC, 0, size, size, &color, 0, 0x40, 1);
            func_800D004C(&state->position, 0xBE, 0xBE, 8, 0, size, size, &color, 0, 0x80, 1);
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            position.y = D_800942EC;
            rotation.x = 0x400;
            rotation.pad = 1;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20(&position, &rotation, size, size, 0x80,
                              GetClut(0x30, palette), 1, 0x40, &color);
            }
        }
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = D_800E27EC << 7;
        RotMatrix(&rotation, &spin);
        matrix = *D_800F32D0->pool->matrix;
        MulMatrix0(&matrix, &spin, &matrix);
        scale.x = scale.y = state->width / 16;
        scale.z = (state->height << 12) / 8000;
        Gte_ScaleMatrix(&matrix, &scale);
        matrix.t[0] = state->position.x;
        matrix.t[1] = state->position.y;
        matrix.t[2] = state->position.z;
        {
            RoomM023BeaconMatrixSlot *slot;
            u16 *index = &D_800E11EA;
            D_800F3368.tpage = D_800E2850[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            slot = &D_800BCFA4;
            gte_ldrotmatrix(slot->value);
            gte_ldtransmatrix(slot->value);
            page = (u16)(D_800E2850[*index] | GetTPage(0, 1, 0, 0));
        }
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            GsSetOrign(page, GetClut(0x50, palette));
        }
        func_800C6ED8(1);
        func_800C6EF8(D_80190760);
        func_800C71E4(D_80190760, &matrix);
        func_800C6F4C(D_80190760);
        func_800C6EF8(D_80190760);
        scale.z = 0x1000;
        scale.y = 0x1800;
        scale.x = 0x1800;
        Gte_ScaleMatrix(&matrix, &scale);
        func_800C70EC(D_80190760, -0xFF, -0x8C, -0xFF);
        func_800C71E4(D_80190760, &matrix);
        func_800C6F4C(D_80190760);
        func_800C6EF8(D_80190760);
        scale.z = 0x1000;
        scale.y = 0x800;
        scale.x = 0x800;
        matrix.t[1] = D_800942EC;
        Gte_ScaleMatrix(&matrix, &scale);
        func_800C6FA0(D_80190760, 0x19);
        func_800C71E4(D_80190760, &matrix);
        func_800C6F4C(D_80190760);
        break;
    }
    return 0;
}
