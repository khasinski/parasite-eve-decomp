/* MASPSX_FLAGS: --expand-div */
#include "pe1/gte.h"
#include "pe1/room_m023_effects.h"

int func_8018F710(int mode, RoomM023JointGlow *glow) {
    GteShortVector position;
    GteRotation rotation;
    RoomM023Template template = D_8018EFF4;
    RenderColor ring = D_8018EFFC;
    RenderColor flare = D_8018F000;
    int size;
    int intensity;
    s16 frame;
    int kind;
    int palette;

    switch (mode) {
    case 0:
        switch (D_800E2368->variant) {
        case 0:
            glow->joint = 8;
            func_800D3F64(0x5AE, func_800D3FD8());
            func_800D3F64(0x5AF, 0x80);
            break;
        case 1:
            glow->joint = 16;
            break;
        }
        glow->state = 0;
        glow->frame = 0;
        return 0;
    case 1:
        glow->frame++;
        if (glow->state >= 3) return 1;
        break;
    case 2:
        size = 0;
        intensity = 0;
        switch (glow->state) {
        case 0:
            frame = glow->frame;
            func_800D0728(&position, 0x258, 0x320, 0x20, 0, frame, frame, &ring, 0,
                          rsin(frame << 7) / 32, 1);
            size = glow->frame << 8;
            intensity = 0x50;
            if (glow->frame >= 8) {
                glow->state = 1;
                glow->frame = 0;
            }
            break;
        case 1:
            frame = glow->frame;
            size = frame * 64 + 0x800;
            intensity = frame * 48 / 32 + 0x50;
            if (frame >= 0x20) {
                glow->state = 2;
                glow->frame = 0;
            }
            break;
        case 2:
            intensity = 0xA0;
            size = rcos(glow->frame << 7) / 2 + 0x1000;
            if (glow->frame >= 8) {
                glow->state = 3;
                glow->frame = 0;
            }
            break;
        }
        D_800F3368.parameter00 = 0x40;
        D_800F3368.extent_x = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_y = 0x40;
        kind = D_800E11EA;
        D_800F3368.tpage = D_800E2850[kind];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0x18;
        if (D_800E27EC & 1) {
            size = size * 5 / 4;
        }
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = -D_800E27EC << 6;
        rotation.flags = 0;
        func_800CE8F0(D_800F32D0->pool, glow->joint, &template, &position);
        {
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            func_800CEE20(&position, &rotation, size * 3 / 2, size * 3 / 2, 0x80,
                          GetClut(0x30, palette), 1, intensity / ((D_800E27EC & 1) + 1), 0);
        }
        {
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            func_800CEE20(&position, &rotation, size * 3 / 2, size * 3 / 2, 0x84,
                          GetClut(0x40, palette), 1, intensity, 0);
        }
        func_800D004C(&position, 0x258, 0x258, 0xC, 0, size, size, &flare, 0, 0x40, 1);
        break;
    }
    return 0;
}
