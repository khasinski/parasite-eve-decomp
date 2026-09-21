#include "pe1/render_object.h"
#include "pe1/field_anim.h"
#include "pe1/random.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"

int func_800D8D14(int mode, GteShortVector *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color;
    int scale, palette;
    switch (mode) {
    case 1:
        state->y -= 3;
        if (D_800E27EC >= 12)
            return 1;
        break;
    case 2:
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = D_800E27EC << 6;
        rotation.flags = 0;
        func_800CF3AC(D_800E1AA0, &color, D_800E27EC);
        scale = 4096 - (D_800E27EC << 12) / 12;
        palette = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(&position, &rotation, scale, scale, 138,
                     GetClut(112, palette), 1, 128, &color);
        break;
    }
    return 0;
}

int func_800D8E74(int mode, RenderHelicalEffect *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color = D_800C22E0;
    GteShortVector *particle;
    int scale, palette, interval;
    /* Matching debt: retail compares the palette selector against $3. */
    register int special_palette asm("$3");
    switch (mode) {
    case 1:
        state->x = D_800E2200.x + rcos(state->angle) * state->radius / 4096;
        state->z = D_800E2200.z + rsin(state->angle) * state->radius / 4096;
        if (state->direction)
            state->y = D_800E2200.y - D_800E27EC * 500 / 36;
        else
            state->y = (s16)(D_800E27EC * 500 / 36 - 1000) + D_800E2200.y;
        state->angle += 96;
        state->radius = rcos((D_800E27EC << 10) / 36) * 700 / 4096;
        if ((D_800E27EC & 1) == 0) {
            particle = func_800CE610(D_800E2208);
            if (particle) {
                particle->x = state->x + (rand() & 7) - 3;
                particle->y = state->y + (rand() & 7) - 3;
                particle->z = state->z + (rand() & 7) - 3;
            }
        }
        state->timer++;
        if (D_800E27EC >= 36)
            return 1;
        break;
    case 2:
        switch (state->stage) {
        case 0:
            rsin((state->timer << 10) / 12);
            scale = 4096;
            if (state->timer >= 12) {
                state->timer = 0;
                state->stage = 1;
            }
            break;
        case 1:
            scale = 4096;
            if (state->timer >= 12) {
                state->timer = 0;
                state->stage = 2;
            }
            break;
        default:
            scale = rcos((state->timer << 10) / 12);
            state->radius += state->timer * 4;
            break;
        }
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = D_800E27EC << 7;
        rotation.flags = 0;
        scale *= 2;
        interval = D_800F336C;
        special_palette = 4;
        palette = D_800E1204[interval];
        if (interval == special_palette && D_800F3428)
            palette += 6;
        else
            palette += 2;
        func_800CEE20(&position, &rotation, scale, scale, 228,
                     GetClut(0, palette), 1, 128, &color);
        break;
    }
    return 0;
}
