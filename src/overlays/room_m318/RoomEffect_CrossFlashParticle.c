#include "pe1/room_shake_burst.h"
#include "pe1/gte.h"

/* Cross flash particle: the flash draws as a pulsing tilted ring, the
 * sparks spin their orientation by their offset and draw as flickering
 * beams from the cross flash centre. */
int func_8019646C(int mode, RoomCrossFlashParticle *p) {
    GteShortVector origin;
    RenderColor glowColor;
    RenderColor color = D_8018F1FC;
    int angle;
    int size;
    int glow;
    int length;
    int width;
    int u;
    int v;

    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            if ((s16)p->timer < 0x20) break;
            return 1;
        case 1:
            p->timer++;
            p->rotation.x += p->x;
            p->rotation.y += p->y;
            p->rotation.z += p->z;
            if ((s16)p->timer < 0x28) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0:
            size = func_80077CF4((s16)p->timer << 5);
            func_800CF3AC(D_80199798, &glowColor, (s16)p->timer);
            func_800D0728((GteShortVector *)p, 0x578, 0x898, 0xC, &p->rotation, size,
                          size, 0, &glowColor, 0x80, 1);
            break;
        case 1: {
            int kind;
            int palette;
            angle = ((s16)p->timer << 10) / 40;
            glow = func_80077DC4(angle) / 32;
            if (p->timer & 1)
                glow = glow * 2 / 3;
            origin.x = D_80199930.x;
            origin.y = D_80199930.y;
            origin.z = D_80199930.z;
            length = 0x76C;
            if ((s16)p->timer < 0x21)
                length = func_80077CF4((s16)p->timer << 5) * 1900 / 4096;
            width = func_80077DC4(angle) / 64 + 0x20;
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            /* The flicker frame reuses the flash size temporary. */
            size = (s16)(p->timer & 3);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            u = (size & 1) * 128;
            v = (size / 2) * 16 + 0x80;
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800D2370(&origin, &p->rotation, length, width, u, v, 0x80, 0x10,
                          GetClut(0x50, palette), &color, &color, (s16)glow, 1);
            break;
        }
        }
        break;
    }
    return 0;
}
