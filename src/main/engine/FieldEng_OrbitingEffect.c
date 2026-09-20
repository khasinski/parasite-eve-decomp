/* MASPSX_FLAGS: --expand-div */
#include "pe1/render_object.h"
#include "pe1/field_anim.h"
#include "pe1/random.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"
int func_800D7FBC(int mode, RenderOrbitingEffect *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color = D_800C22E0;
    GteShortVector *particle;
    int scale, intensity, palette, interval;
    /* Matching debt: retail compares the palette selector against $3. */
    register int special_palette asm("$3");
    switch (mode) {
    case 1:
        state->x = D_800E21EC.x + rcos(state->angle) * state->radius / 4096;
        state->z = D_800E21EC.z + rsin(state->angle) * state->radius / 4096;
        state->angle += 96;
        state->radius = rcos((D_800E27EC << 10) / 36) * 700 / 4096;
        interval = D_800E2368->variables[6] == 11 ? 1 : 3;
        if (D_800E27EC % interval == 0) {
            particle = func_800CE610(D_800E21F4);
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
            intensity = rsin((state->timer << 10) / 12) / 32;
            scale = 4096;
            if (state->timer >= 12) {
                state->timer = 0;
                state->stage = 1;
            }
            break;
        case 1:
            scale = 4096;
            intensity = 128;
            if (state->timer >= 12) {
                state->timer = 0;
                state->stage = 2;
            }
            break;
        default:
            intensity = 128;
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
            palette += 7;
        else
            palette += 3;
        func_800CEE20(&position, &rotation, scale, scale, 36,
                     GetClut(0, palette), 1, intensity, &color);
        break;
    }
    return 0;
}
