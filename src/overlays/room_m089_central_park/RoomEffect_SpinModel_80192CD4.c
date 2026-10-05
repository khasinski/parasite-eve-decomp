#include "common.h"
#include "pe1/room_m089_spin_model.h"

/* Spinning model: loads its model, swells a glow over the anchor for two
 * stages and draws the model twice, the inner copy at half scale. The tile
 * index and tpage table go through local pointers (retail keeps both in
 * saved registers, which spills the model intensity to the frame), and the
 * palette kind/palette pair is block-local per draw. */

int func_80192CD4(int mode, RoomM089SpinModel *spin, GteShortVector *anchor) {
    GteShortVector position;
    GteShortVector rotation;
    RenderColor color = D_8018F1D4;
    GteMatrix matrix;
    GteVector scale;
    GteMatrix innerMatrix;
    GteVector innerScale;
    int intensity;
    int size;
    int glow;
    int turn;
    int page;
    u16 *index;
    u16 *tpages;

    switch (mode) {
    case 0:
        spin->stage = 0;
        spin->timer = 0;
        D_80194168 = func_8006E498(D_800B0E64.channel, 0xC5864704);
        func_800C6D5C(D_80194168, 0, 0);
        return 0;
    case 1:
        spin->timer++;
        if (spin->stage < 2) break;
        return 1;
    case 2:
        size = 0;
        glow = 0;
        turn = 0;
        intensity = 0;
        switch (spin->stage) {
        case 0:
            turn = func_80077DC4(spin->timer << 6);
            intensity = func_80077CF4(spin->timer << 7) / 32;
            glow = 0x40;
            size = func_80077CF4(spin->timer << 6);
            if (spin->timer >= 16) {
                spin->stage = 1;
                spin->timer = 0;
            }
            break;
        case 1:
            glow = func_80077DC4(spin->timer << 6) / 32 * 3 / 2;
            size = 0x1000;
            if (spin->timer >= 16) {
                spin->stage = 2;
                spin->timer = 0;
            }
            break;
        }
        index = &D_800E11FA;
        tpages = D_800E2850;
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.tpage = D_800E2850[*index];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 2;
        position.x = anchor->x;
        position.y = anchor->y;
        position.z = anchor->z;
        if (size != 0) {
            rotation.x = 0x400;
            rotation.y = 0;
            rotation.z = -D_800E27EC << 6;
            rotation.pad = 1;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                func_800CEE20(&position, &rotation, size, size, 0x26,
                              GetClut(0, (kind == 4 && D_800F3428) ? palette + 9 : palette + 5),
                              1, glow, &color);
            }
        }
        rotation.x = 0;
        rotation.z = 0;
        position.y -= (0x20 - D_800E27EC) << 4;
        turn = turn * 2 / 3;
        rotation.y = -(D_800E27EC << 6);
        D_800F3368.tpage = tpages[*index];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 1;
        page = (u16)(tpages[*index] | GetTPage(0, 1, 0, 0));
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            GsSetOrign(page, GetClut(0, (kind == 4 && D_800F3428) ? palette + 7 : palette + 3));
        }
        func_800C6ED8(1);
        RotMatrixYXZ(&rotation, &matrix);
        matrix.t[0] = position.x;
        matrix.t[1] = position.y;
        matrix.t[2] = position.z;
        scale.x = turn;
        scale.y = turn;
        scale.z = turn;
        Gte_ScaleMatrix(&matrix, &scale);
        func_800C6EF8(D_80194168);
        func_800C6FA0(D_80194168, intensity);
        func_800C71E4(D_80194168, &matrix);
        func_800C6F4C(D_80194168);
        position.y += (0x20 - D_800E27EC) << 3;
        RotMatrixYXZ(&rotation, &innerMatrix);
        innerMatrix.t[0] = position.x;
        innerMatrix.t[1] = position.y;
        innerMatrix.t[2] = position.z;
        innerScale.x = turn / 2;
        innerScale.y = turn / 2;
        innerScale.z = turn / 2;
        Gte_ScaleMatrix(&innerMatrix, &innerScale);
        func_800C6EF8(D_80194168);
        func_800C6FA0(D_80194168, intensity);
        func_800C71E4(D_80194168, &innerMatrix);
        func_800C6F4C(D_80194168);
        break;
    }
    return 0;
}
