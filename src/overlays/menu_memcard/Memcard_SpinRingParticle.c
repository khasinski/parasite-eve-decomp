#include "menu_memcard_glint.h"

/* Particle that spins: state 0 draws a halo, state 1 a tilted textured
 * ring around the shared origin, state 2 a scaled model that orbits it. */
int Memcard_SpinRingParticle(int mode, MemcardSpinParticle *p) {
    GteShortVector offset;
    RenderColor color;
    RenderColor colorA = D_801ED848;
    RenderColor colorB = D_801ED84C;
    MemcardModelMatrix matrix;
    MemcardModelScale scale;
    RenderMatrixSlot *matrixSlot;
    int angle;
    int size;
    int width;
    int height;
    int fade;
    int u;
    int v;

    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            if ((s16)p->timer < 32) break;
            return 1;
        case 1:
            p->timer++;
            p->rotation.x += p->x;
            p->rotation.y += p->y;
            p->rotation.z += p->z;
            if ((s16)p->timer < 40) break;
            return 1;
        case 2:
            p->timer++;
            p->rotation.z += 44;
            if ((s16)p->timer < 32) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0:
            size = rsin((s16)p->timer << 5);
            func_800CF3AC(D_801F1CD8, &color, (s16)p->timer);
            func_800D0728((GteShortVector *)p, 1400, 2200, 18, &p->rotation, size, size,
                          0, &color, 128, 1);
            break;
        case 1: {
            int kind;
            int palette;
            angle = ((s16)p->timer << 10) / 40;
            fade = rcos(angle) / 32;
            if (p->timer & 1) {
                fade = fade * 2 / 3;
            }
            offset.x = D_801F1F28.x;
            offset.y = D_801F1F28.y;
            offset.z = D_801F1F28.z;
            width = 1900;
            if ((s16)p->timer < 33) {
                width = rsin((s16)p->timer << 5) * 1900 / 4096;
            }
            height = rcos(angle) / 32 + 64;
            matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            size = (s16)(p->timer & 3);
            u = (size & 1) << 7;
            v = (size / 2) * 16 + 160;
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800D2370(&offset, &p->rotation, width, height, u, v, 128, 16,
                          func_80077AA4(96, palette), &colorA, &colorB, (s16)fade, 1);
            break;
        }
        case 2: {
            int kind;
            int palette;
            int page;
            u16 *index;
            int spin;
            spin = (s16)p->timer << 5;
            width = rsin(spin) / 8 + 100;
            func_800CFB7C((GteShortVector *)&p->rotation, (s16)width, &offset);
            offset.x += D_801F1F28.x;
            offset.y += D_801F1F28.y;
            offset.z += D_801F1F28.z;
            size = rsin(spin) * 2 + 0x800;
            fade = rcos(spin) / 32;
            index = &D_800E11E6;
            D_800F336C = 1;
            D_800F3368.tpage = D_800E2850[*index];
            func_800CEDA8(1);
            D_800F3368.parameter06 = 0;
            page = (D_800E2850[*index] | func_80077A64(0, 1, 0, 0)) & 0xFFFF;
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800C6EC0(page, func_80077AA4(224, palette));
            func_800C6ED8(1);
            func_80079754(&p->rotation, &matrix);
            matrix.t[0] = offset.x;
            matrix.t[1] = offset.y;
            matrix.t[2] = offset.z;
            scale.x = size;
            scale.y = size;
            scale.z = size;
            func_80078CC4(&matrix, &scale);
            func_800C6EF8(D_800E22D4);
            func_800C6FA0(D_800E22D4, (u16)fade);
            func_800C71E4(D_800E22D4, &matrix);
            func_800C6F4C(D_800E22D4);
            break;
        }
        }
        break;
    }
    return 0;
}
