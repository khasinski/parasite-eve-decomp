#include "pe1/room_m256.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"

int func_8019552C(int mode, RoomM256Particle *particle) {
    GteShortVector position;
    GteShortVector color;
    int scale;
    int palette;
    int kind;
    u16 clut;

    switch (mode) {
    case 1:
        particle->frame++;
        particle->offset += 8;
        if ((s16)particle->frame >= 16) return 1;
        break;
    case 2:
        scale = particle->scale * func_80077DC4((s16)particle->frame << 6) / 4096;
        func_800CF844(D_801960A0, &position, scale, D_80196098,
                      scale, (s16)particle->offset);
        func_800CF3AC(D_80195E64, &color, 0x30 - (scale * 48) / 1024);
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428 != 0) palette += 4;
        clut = func_80077AA4(0x20, palette);
        func_800CEE20(&position, 0, 0x1000, 0x1000,
                       D_800F336A * 2 + 0xD8, clut, 3,
                       D_80196094, &color);
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        func_800D1DEC(&position, &color, D_80196094, 1);
        break;
    }
    return 0;
}
