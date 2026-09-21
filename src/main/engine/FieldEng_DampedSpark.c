#include "pe1/render_object.h"
#include "pe1/field_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/random.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"

int func_800D9554(int mode, RenderDampedSpark *state)
{
    GteShortVector position;
    u32 color;
    switch (mode) {
    case 1:
        state->x += state->vx + (rand() & 3) - 1;
        state->y += state->vy + (rand() & 3) - 1;
        state->z += state->vz;
        state->vx = state->vx * 31 / 32;
        state->vz = state->vz * 31 / 32;
        if (D_800E27EC < 19)
            state->vy--;
        else
            state->vy++;
        if (D_800E27EC >= 40)
            return 1;
        break;
    case 2:
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        if (D_800E27EC < 19)
            func_800CF3AC(D_800E1AC8, &color, D_800E27EC);
        else if ((rand() & 3) == 0)
            color = 0xC8C8;
        else
            color = 0;
        func_800D1DEC(&position, &color, 128, 1);
        break;
    }
    return 0;
}

int func_800D96F4(int mode, RenderSparkEmitter *state)
{
    GteRotation rotation;
    RenderColor color = D_800C22E4;
    RenderDampedSpark *effect;
    int count, speed, scale, intensity, palette, selector;
    register int special_palette asm("$3");
    register int x asm("$12");
    register int y asm("$13");
    register int z asm("$14");
    s32 **slot;
    switch (mode) {
    case 0:
        state->phase = rand();
        func_800CE870((char *)D_8009D254, 0, &state->position.x);
        return func_800CE560(D_800F33E0->end, 12, 24,
                            (FieldAnimTaskCallback)func_800D9554);
    case 1:
        if (D_800E27EC < 12) {
            for (count = 0; count < 2; ++count) {
                effect = func_800CE610(D_800F33E0->end);
                if (effect) {
                    effect->x = state->position.x;
                    effect->y = state->position.y;
                    effect->z = state->position.z;
                    speed = (rand() & 7) + 43;
                    effect->vx = rsin(state->phase) * speed / 4096;
                    effect->vz = rcos(state->phase) * speed / 4096;
                    effect->vy = (rand() & 31) - 10;
                    state->phase += 0x955;
                }
            }
        }
        if (D_800E27EC >= 70)
            return 1;
        break;
    case 2:
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11F6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 100;
        if (D_800E27EC < 33) {
            intensity = rcos(D_800E27EC << 5) / 32;
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = D_800E27EC << 4;
            rotation.flags = 0;
            scale = rsin(D_800E27EC << 5) + 4096;
            /* Matching debt: complete the scale before palette selection. */
            asm volatile("" : "=r"(scale) : "0"(scale));
            selector = D_800F336C;
            special_palette = 4;
            palette = D_800E1204[selector];
            if (selector == special_palette && D_800F3428)
                palette += 7;
            else
                palette += 3;
            func_800CEE20(&state->position, &rotation, scale, scale, 66,
                         GetClut(0, palette), 1, intensity, &color);
        }
        {
            register GteMatrixWords *matrix asm("$8");
            slot = &D_800BCFA4.value;
            asm volatile("" : "=r"(slot) : "0"(slot));
            matrix = (GteMatrixWords *)*slot;
            x = matrix->r11_r12;
            y = matrix->r13_r21;
            gte_ctc2_0(x);
            gte_ctc2_1(y);
            x = matrix->r22_r23;
            y = matrix->r31_r32;
            z = matrix->r33_pad;
            gte_ctc2_2(x);
            gte_ctc2_3(y);
            gte_ctc2_4(z);
            x = matrix->tx;
            y = matrix->ty;
            gte_ctc2_5(x);
            z = matrix->tz;
            gte_ctc2_6(y);
            gte_ctc2_7(z);
        }
        D_800F3368.depth = 8;
        break;
    }
    return 0;
}
