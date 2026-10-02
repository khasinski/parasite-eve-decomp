#include "pe1/gte.h"
#include "pe1/room_m123.h"

int func_80192BDC(int mode, RoomM123PulsingParticle *particle) {
    GteShortVector position;
    GteShortVector output;
    GteShortVector color;
    int scale;
    int palette;
    int kind;
    int blend;
    u16 clut;
    RenderMatrixSlot *matrixSlot;

    switch (mode) {
    case 1:
        particle->frame++;
        particle->offset += 16;
        if ((s16)particle->frame >= 48) return 1;
        break;
    case 2:
        scale = particle->scale *
                func_80077DC4(((s16)particle->frame << 10) / 48) / 4096;
        func_800CF844(D_80195690, &position, scale / 2, D_80195688,
                      scale, (s16)particle->offset);
        if ((s16)particle->frame < 16) {
            func_800CF3AC(D_801954BC, &color, 0x30 - (scale * 48) / 1024);
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            clut = func_80077AA4(0x20, palette);
            func_800CEE20(&position, 0, 0x1000, 0x1000,
                          D_800F336A * 2 + 0xD8, clut, 3, D_80195684,
                          &color);
            matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            func_800D1DEC(&position, &color, D_80195684, 1);
            /* Retail re-tests the frame here; the blended tail only runs
             * from frame 16 on. */
            if ((s16)particle->frame < 16) break;
        }
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        func_800CF3AC(D_801954BC, &color, 0x30 - (scale * 48) / 1024);
        blend = func_80077DC4(((s16)particle->frame - 16) << 5);
        func_800783E4(&position, D_80195690, blend, 0x1000 - blend, &output);
        func_800D2B58(&output, &position, &color, 0, D_80195684, 0, 1);
        break;
    }
    return 0;
}
