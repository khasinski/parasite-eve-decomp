#include "pe1/render_object.h"
#include "pe1/field_anim.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_prim.h"
#include "pe1/random.h"
#include "pe1/battle_runtime.h"

int func_800DC058(int mode, FieldAnimTwinModel *state)
{
    GteShortVector angles;
    GteShortVector position;
    GteVector scale;
    RenderColor color;
    GteMatrix matrix;
    int height;
    int spin;
    int size;

    switch (mode) {
    case 0:
        state->angle = rand();
        state->stage = 0;
        state->timer = 0;
        func_800CE870((char *)D_8009D254, 1, &state->position.x);
        func_800C6D5C(D_800F3418, 0, 0);
        func_800C6D5C(D_800F342C, 0, 0);
        break;
    case 1:
        state->timer++;
        if (D_800E27EC >= 64)
            return 1;
        break;
    case 2:
        spin = 0;
        state->angle += 16;
        switch (state->stage) {
        case 0:
            height = (state->timer << 12) / 24;
            if (state->timer >= 24) {
                state->stage = 1;
                state->timer = 0;
            }
            break;
        case 1:
            height = 0x1000;
            spin = ((state->timer / 2 + 1) << 12) / 6;
            if (state->timer >= 12) {
                state->timer = 0;
                state->stage = 2;
            }
            break;
        default:
            height = 0x1000;
            break;
        }
        scale.z = 0x44C;
        scale.x = 0x44C;
        scale.y = height / 4;
        angles.x = 0;
        angles.y = state->angle;
        angles.z = 0;
        func_800CF3AC(D_800E1E24, &color, D_800E27EC);
        RotMatrixYXZ(&angles, &matrix);
        Gte_ScaleMatrix(&matrix, &scale);
        matrix.t[0] = state->position.x;
        matrix.t[1] = state->position.y;
        matrix.t[2] = state->position.z;
        D_800F3368.tpage = D_800E2850[D_800E11E4[9]];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        {
            int tpage = (u16)(D_800E2850[D_800E11E4[9]] | GetTPage(0, 3, 0, 0));
            int kind = D_800F336C;
            int palette = D_800E1204[kind];
            GsSetOrign(tpage, GetClut(0, (kind == 4 && D_800F3428) ? palette + 10 : palette + 6));
        }
        func_800C6ED8(1);
        func_800C6EF8(D_800F3418);
        func_800C7098(D_800F3418, color.r, color.g, color.b);
        func_800C71E4(D_800F3418, &matrix);
        size = 0x4B0;
        scale.z = size;
        scale.x = size;
        angles.x = 0x800;
        angles.y = state->angle;
        angles.z = 0;
        position.x = state->position.x;
        position.y = state->position.y;
        position.z = state->position.z;
        position.y -= 1000;
        RotMatrixYXZ(&angles, &matrix);
        Gte_ScaleMatrix(&matrix, &scale);
        matrix.t[0] = position.x;
        matrix.t[1] = position.y;
        matrix.t[2] = position.z;
        func_800C71E4(D_800F3418, &matrix);
        func_800C6F4C(D_800F3418);
        if (spin) {
            scale.z = size;
            scale.x = size;
            angles.x = 0;
            angles.y = state->angle + spin;
            angles.z = 0;
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            RotMatrixYXZ(&angles, &matrix);
            Gte_ScaleMatrix(&matrix, &scale);
            matrix.t[0] = position.x;
            matrix.t[1] = position.y;
            matrix.t[2] = position.z;
            {
                int tpage = (u16)(D_800E2850[D_800E11E4[9]] | GetTPage(0, 1, 0, 0));
                int kind = D_800F336C;
                int palette = D_800E1204[kind];
                GsSetOrign(tpage, GetClut(0, (kind == 4 && D_800F3428) ? palette + 10 : palette + 6));
            }
            func_800C6ED8(1);
            func_800C6EF8(D_800F342C);
            func_800C7098(D_800F342C, 255, 255, 255);
            func_800C71E4(D_800F342C, &matrix);
            func_800C6F4C(D_800F342C);
        }
        break;
    }
    return 0;
}
