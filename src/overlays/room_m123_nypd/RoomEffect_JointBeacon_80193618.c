#include "common.h"
#include "pe1/room_m123_beacon.h"

/* Joint beacon: rides joint 0x17, swells in, pulses and fades out, tagging
 * the player once on contact; draws a glow sprite pair with two star fans,
 * a fading three-sprite ring on the ground, a turning model while the effect
 * starts and three scaled passes of a second model. */
int func_80193618(int mode, RoomM123Beacon *state) {
    GteVector scale;
    GteShortVector position;
    GteShortVector rotation;
    RoomM123BeaconOffset offset = D_8018F1E8;
    GteMatrix matrix;
    GteMatrix spin;
    RoomM123BeaconColor color;
    int size;
    int page;

    switch (mode) {
    case 0:
        state->fade = 200;
        state->phase = 0;
        state->timer = 0;
        state->glow = 0;
        state->width = 0;
        state->depth = 0x1000;
        if (D_800E2368->active) {
            RoomM123BeaconPool *pool = D_800F32D0->pool;
            if (pool && pool->node) {
                u8 *flag = pool->node->state;
                if (*flag == 1) *flag = 2;
            }
        }
        state->armed = 1;
        D_80195698 = func_8006E498(D_800B0E64.channel, 0xC5483704);
        D_8019569C = func_8006E498(D_800B0E64.channel, 0xC5883704);
        func_800C6D5C(D_80195698, 0, 0);
        func_800C6D5C(D_8019569C, 0, 0xF0);
        return 0;
    case 1:
        func_800CE8F0(D_800F32D0->pool, 0x17, &offset, state);
        func_800CE9D4(D_800F32D0->pool, 0, &state->rotation);
        state->alpha = 0x80;
        switch (state->phase) {
        case 0:
            state->timer++;
            state->height = func_80077DC4(state->timer << 7) / 2 + 0x1000;
            if (state->width < 0x800) state->width += 0x100;
            state->depth -= 0x80;
            state->glow = func_80077DC4(state->timer << 7) + 0x1000;
            state->shade = state->timer;
            if (state->timer >= 8) {
                state->phase = 1;
                state->timer = 0;
            }
            break;
        case 1:
            state->timer++;
            state->height = 0x1000;
            state->glow = ((D_800E27EC & 1) << 9) + 0x1000;
            if (state->shade < 0x20) state->shade++;
            if (state->timer >= 0x20) {
                state->phase = 2;
                state->timer = 0;
            }
            break;
        case 2:
            state->height = 0x1000;
            state->width = 0x800;
            state->glow = 0x1000;
            state->timer++;
            state->shade++;
            state->alpha = 0x80 - state->timer * 8;
            if (state->timer >= 0x10) return 1;
            break;
        }
        func_800CFB7C(&state->rotation, 0x7D0, &position);
        state->target.x = state->position.x;
        state->target.y = state->position.y;
        state->target.z = state->position.z;
        state->target.x += position.x;
        state->target.y += position.y;
        state->target.z += position.z;
        if (func_800CEB8C(state, &state->target, 0x140) == 0) return 0;
        if (state->armed == 0) return 0;
        if (D_800E2368->active) {
            RoomM123BeaconChannel *channel = D_800F32D0;
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
        D_800F3368.depth = 0x32;
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        D_800F3368.tpage = D_800E2850[D_800E11FA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        if (state->glow != 0) {
            rotation.x = 0;
            rotation.y = 0;
            rotation.pad = 0;
            rotation.z = D_800E27EC << 6;
            size = state->glow;
            func_800CF3AC(D_8019550C, &color, state->shade);
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                func_800CEE20(&state->position, &rotation, size * 3 / 2, size * 3 / 2, 0x50,
                              GetClut(0, (kind == 4 && D_800F3428) ? palette + 6
                                                                  : palette + 2),
                              1, 0xA0, &color);
            }
            func_800D004C(&state->position, 0x2BC, 0x2BC, 0xC, 0, size, size, &color, 0, 0x50, 1);
            func_800D004C(&state->position, 0xF0, 0xF0, 8, 0, size, size, &color, 0, 0x80, 1);
            if (state->fade > 0) {
                state->fade -= 8;
                if (state->fade > 0) {
                    position.x = state->position.x;
                    position.y = state->position.y;
                    position.z = state->position.z;
                    D_800F3368.parameter00 = 0x20;
                    D_800F3368.parameter02 = 2;
                    D_800F3368.extent_x = 0x20;
                    D_800F3368.extent_y = 0x20;
                    D_800F3368.tpage = D_800E2850[D_800E11E4[3]];
                    D_800F3368.palette = 3;
                    D_800F3368.parameter06 = 0;
                    {
                        int kind = D_800F3368.palette;
                        int palette = D_800E1204[kind];
                        if (kind == 4 && D_800F3428) palette += 4;
                        func_800D3BC8(&position, 0x1000, 0x1000, 0x4D, GetClut(0x50, palette), 1,
                                      state->fade, 0, 0xA00);
                    }
                    D_800F3368.parameter00 = 0x10;
                    D_800F3368.parameter02 = 1;
                    D_800F3368.extent_x = 0x10;
                    D_800F3368.extent_y = 0x10;
                    {
                        int kind = D_800F3368.palette;
                        int palette = D_800E1204[kind];
                        if (kind == 4 && D_800F3428) palette += 4;
                        func_800D3BC8(&position, 0x1000, 0x1000, 0x48, GetClut(0x30, palette), 1,
                                      state->fade, 0, 0xC00);
                    }
                    D_800F3368.parameter00 = 0x40;
                    D_800F3368.parameter02 = 4;
                    D_800F3368.extent_x = 0x40;
                    D_800F3368.extent_y = 0x40;
                    {
                        int kind = D_800F3368.palette;
                        int palette = D_800E1204[kind];
                        if (kind == 4 && D_800F3428) palette += 4;
                        func_800D3BC8(&position, 0x1000, 0x1000, 0x49, GetClut(0x40, palette), 1,
                                      state->fade, 0, 0xE00);
                    }
                }
            }
        }
        {
            u16 *index = &D_800E11EA;
            u16 *tpages = D_800E2850;
            D_800F3368.tpage = D_800E2850[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            if (D_800E27EC < 0x11) {
                RoomM123BeaconMatrixSlot *slot;
                rotation.x = 0x800;
                rotation.y = 0;
                rotation.z = D_800E27EC << 4;
                RotMatrixYXZ(&rotation, &spin);
                matrix = *D_800F32D0->pool->matrix;
                MulMatrix0(&matrix, &spin, &matrix);
                scale.x = scale.y = func_80077CF4(D_800E27EC << 6) / 4 + 0x400;
                scale.z = 0x1000;
                Gte_ScaleMatrix(&matrix, &scale);
                matrix.t[0] = state->position.x;
                matrix.t[1] = state->position.y;
                matrix.t[2] = state->position.z;
                slot = &D_800BCFA4;
                gte_ldrotmatrix(slot->value);
                gte_ldtransmatrix(slot->value);
                D_800F3368.tpage = tpages[*index];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 0;
                page = (u16)(tpages[*index] | GetTPage(0, 1, 0, 0));
                {
                    int kind = D_800F3368.palette;
                    int palette = D_800E1204[kind];
                    if (kind == 4 && D_800F3428) palette += 4;
                    GsSetOrign(page, GetClut(0x60, palette));
                }
                func_800C6ED8(1);
                func_800C6EF8(D_8019569C);
                func_800C7098(D_8019569C, color.r, color.g, color.b);
                size = func_80077DC4(D_800E27EC << 6) / 64;
                func_800C6FA0(D_8019569C, size);
                func_800C71E4(D_8019569C, &matrix);
                func_800C6F4C(D_8019569C);
            }
        }
        rotation.x = 0x800;
        rotation.y = 0;
        rotation.z = D_800E27EC << 7;
        RotMatrixYXZ(&rotation, &spin);
        matrix = *D_800F32D0->pool->matrix;
        MulMatrix0(&matrix, &spin, &matrix);
        scale.x = scale.y = state->height * 2 / 5;
        scale.z = 0x1800;
        Gte_ScaleMatrix(&matrix, &scale);
        matrix.t[0] = state->position.x;
        matrix.t[1] = state->position.y;
        matrix.t[2] = state->position.z;
        {
            RoomM123BeaconMatrixSlot *slot = &D_800BCFA4;
            gte_ldrotmatrix(slot->value);
            gte_ldtransmatrix(slot->value);
        }
        {
            u16 *index = &D_800E11EA;
            D_800F3368.tpage = D_800E2850[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[*index] | GetTPage(0, 3, 0, 0));
        }
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            GsSetOrign(page, GetClut(0x60, palette));
        }
        func_800C6ED8(1);
        func_800C6EF8(D_80195698);
        func_800C7098(D_80195698, color.r, color.g, color.b);
        func_800C71E4(D_80195698, &matrix);
        func_800C6F4C(D_80195698);
        func_800C6EF8(D_80195698);
        scale.z = 0x1000;
        scale.x = scale.y = state->depth;
        Gte_ScaleMatrix(&matrix, &scale);
        {
            u16 *index = &D_800E11EA;
            D_800F3368.tpage = D_800E2850[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[*index] | GetTPage(0, 3, 0, 0));
        }
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            GsSetOrign(page, GetClut(0x60, palette));
        }
        func_800C6ED8(1);
        func_800C7098(D_80195698, color.r, color.g, color.b);
        func_800C71E4(D_80195698, &matrix);
        func_800C6F4C(D_80195698);
        func_800C6EF8(D_80195698);
        scale.z = 0x1000;
        scale.x = scale.y = state->width;
        Gte_ScaleMatrix(&matrix, &scale);
        {
            u16 *index = &D_800E11EA;
            D_800F3368.tpage = D_800E2850[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[*index] | GetTPage(0, 1, 0, 0));
        }
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            GsSetOrign(page, GetClut(0x60, palette));
        }
        func_800C6ED8(1);
        func_800C7098(D_80195698, color.r, color.g, color.b);
        func_800C71E4(D_80195698, &matrix);
        func_800C6F4C(D_80195698);
        break;
    }
    return 0;
}
