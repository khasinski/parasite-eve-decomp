#include "../../../src/overlays/room_m273/room_m273_boss.h"
#include "pe1/gte.h"

/* Ground ring: while it opens (frames up to 24) it knocks the player back
 * once when the player stands inside the pulsing radius; while drawing it
 * shows the swelling ring model, a short flash and two rotating rings. */
int func_80198E94(int mode, GteShortVector *position) {
    GteRotation rotation;
    GteMatrix matrix;
    GteVector scale;
    s16 unused[4];
    int frame;
    int size;
    int fade;
    int value;
    int i;

    if (mode == 1) {
        int now = D_800E27EC;
        if (now >= 0x28) return 1;
        if (now >= 0x19) return 0;
        if (position->pad != 0) return 0;
        if (D_8019AF74.cooldown != 0) return 0;
        i = D_800966EC[((now << 11) / 40) & 0xFFF].sine;
        i /= 16;
        i += 0x80;
        {
            int dx = g_PlayerEntity->position[0] - position->x;
            int dz = g_PlayerEntity->position[2] - position->z;
            if (i < func_8005186C(dx * dx + dz * dz)) return 0;
        }
        position->pad = 1;
        g_PlayerEntity->actor->flags |= 0x4000;
        if (D_800F32D0->instance->owner) {
            D_800F32D0->instance->owner->flags |= 0x80000000;
            D_8019AF74.cooldown = 0x28;
        }
        return 0;
    }
    if (mode != 2) return 0;
    frame = D_800E27EC - 1;
    if ((u32)(D_800E27EC - 5) < 0x20) {
        int page;
        int kind;
        int palette;
        int cosine;
        int sine;
        value = D_800E27EC - 5;
        if (value < 8) value = D_800966EC[(value << 7) & 0xF80].sine;
        else value = D_800966EC[(((D_800E27EC - 13) << 10) / 24) & 0xFFF].cosine;
        page = (D_800E2850[D_800E11FA] | GetTPage(0, 1, 0, 0)) & 0xFFFF;
        fade = value >> 6;
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        size = value >> 2;
        GsSetOrign(page, GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 6 : palette + 2));
        func_800C6ED8(1);
        func_800C6EF8(D_8019AF04.model);
        func_800C6FA0(D_8019AF04.model, (u16)fade);
        cosine = rcos(frame << 8);
        sine = rsin(frame << 8);
        matrix.m[0][2] = sine;
        matrix.m[2][0] = -sine;
        matrix.m[0][0] = cosine;
        matrix.m[2][2] = cosine;
        matrix.t[2] = 0;
        matrix.t[1] = 0;
        matrix.t[0] = 0;
        matrix.m[2][1] = 0;
        matrix.m[1][2] = 0;
        matrix.m[1][0] = 0;
        matrix.m[0][1] = 0;
        matrix.m[1][1] = 0x1000;
        memset(&scale, 0, sizeof(scale));
        scale.x = size;
        scale.y = size;
        scale.z = size;
        Gte_ScaleMatrix(&matrix, &scale);
        matrix.t[0] = position->x;
        matrix.t[1] = position->y;
        matrix.t[2] = position->z;
        func_800C71E4(D_8019AF04.model, &matrix);
        func_800C6F4C(D_8019AF04.model);
    }
    D_800F3368.parameter00 = 0x20;
    D_800F3368.parameter02 = 2;
    D_800F3368.extent_x = 0x20;
    D_800F3368.extent_y = 0x20;
    D_800F3368.extent_x = 0x20;
    D_800F3368.extent_y = 0x20;
    {
        int tpage = D_800E2850[D_800E11EA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.tpage = tpage;
    }
    if (frame < 8) {
        int kind = D_800F3368.palette;
        int palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428 != 0) palette += 4;
        size = D_800966EC[(frame << 7) & 0xF80].sine + 0x1800;
        func_800CEE20(position, 0, size, size, (s16)D_800F3368.parameter02 * frame + 0x80,
                      (u16)GetClut(0x10, palette), 1, 0x80, 0);
    }
    rotation.x = 0x400;
    rotation.y = -frame << 9;
    rotation.flags = 1;
    rotation.z = 0;
    fade = D_800966EC[(frame * 25) & 0xFFF].cosine >> 5;
    size = D_800966EC[(frame * 51) & 0xFFF].sine / 16;
    for (i = 0; i < 2; i++) {
        func_800D004C(position, size, size, 8, &rotation, 0x1000, 0x1000,
                      &D_8019AE1C[0], &D_8019AE1C[1], fade, 1);
        size += 0x100;
    }
    return 0;
}
