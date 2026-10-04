#include "common.h"
#include "pe1/field_fade_particle.h"

/* Phase 0 follows actor matrix point `kind` and every fourth frame sheds a
 * falling spark (phase 2, one in three) or a drifting one (phase 1);
 * phases 1 and 2 drift for 16 and 24 frames. Mode 2 draws each phase as a
 * shape quad capped at the emitter's intensity D_800E2244. */
int func_800DEFFC(int mode, RenderFadeParticle *particle)
{
    GteShortVector offset = D_800C22F0;
    RenderColor color;
    RenderFadeParticle *child;
    int scale;
    int intensity;
    int kind;
    int palette;

    switch (mode) {
    case 1:
        switch (particle->phase) {
        case 0:
            particle->timer++;
            FieldEng_TransformMatrixPoint((struct RoomFxTransformOwner *)D_8009D254,
                                          particle->kind, &offset,
                                          (GteShortVector *)particle);
            if (D_800E27EC & 3)
                return 0;
            if (rand() % 3 == 0) {
                child = func_800CE610(D_800F33E0->end);
                if (child == 0)
                    break;
                child->position[0] = particle->position[0];
                child->position[1] = particle->position[1];
                child->position[2] = particle->position[2];
                child->vx = (rand() & 7) - 3;
                child->vz = (rand() & 7) - 3;
                child->vy = (rand() & 0xF) - 12;
                child->phase = 2;
                child->timer = 0;
            } else {
                child = func_800CE610(D_800F33E0->end);
                if (child == 0)
                    return 0;
                child->position[0] = particle->position[0];
                child->position[1] = particle->position[1];
                child->position[2] = particle->position[2];
                child->position[0] += (rand() & 0x7F) - 0x40;
                child->position[1] += (rand() & 0x7F) - 0x40;
                child->position[2] += (rand() & 0x7F) - 0x40;
                child->vx = (rand() & 0xF) - 7;
                child->vz = (rand() & 0xF) - 7;
                child->vy = (rand() & 7) - 11;
                child->phase = 1;
                child->timer = 0;
            }
            break;
        case 1:
            particle->timer++;
            particle->position[0] += particle->vx;
            particle->position[1] += particle->vy;
            particle->position[2] += particle->vz;
            particle->position[0] += (rand() & 7) - 3;
            particle->position[1] += (rand() & 7) - 3;
            particle->position[2] += (rand() & 7) - 3;
            if (particle->timer < 16)
                break;
            return 1;
        case 2:
            particle->timer++;
            particle->position[0] += particle->vx;
            particle->position[1] += particle->vy;
            particle->position[2] += particle->vz;
            if (particle->timer < 24)
                break;
            return 1;
        default:
            return 0;
        }
        break;
    case 2:
        switch (particle->phase) {
        case 0:
            scale = 0x1800;
            if (particle->timer < 33)
                scale = rsin(particle->timer << 5) + 0x800;
            if (particle->timer < 25) {
                intensity = rsin((particle->timer << 10) / 24) / 32;
            } else if (particle->timer & 1) {
                intensity = 0x80;
            } else {
                intensity = 0x64;
            }
            if (D_800E2244 < intensity)
                intensity = D_800E2244;
            func_800CEE20((GteShortVector *)particle, 0, scale / 2, scale / 2,
                          (s16)D_800F3368.parameter02 * (particle->timer & 3) + 0x68,
                          GetClut(16, (D_800F336C == 4 && D_800F3428)
                                      ? D_800E1204[D_800F336C] + 4
                                      : D_800E1204[D_800F336C]),
                          1, intensity, 0);
            break;
        case 1:
            intensity = particle->timer << 6;
            scale = rcos(intensity) / 2 + 0x800;
            intensity = rcos(intensity) / 32;
            if (D_800E2244 < intensity)
                intensity = D_800E2244;
            func_800CEE20((GteShortVector *)particle, 0, scale, scale,
                          (s16)D_800F3368.parameter02 * (particle->timer & 3) + 0x68,
                          GetClut(16, (D_800F336C == 4 && D_800F3428)
                                      ? D_800E1204[D_800F336C] + 4
                                      : D_800E1204[D_800F336C]),
                          1, intensity / 2, 0);
            break;
        case 2:
            scale = rsin((particle->timer << 10) / 24) + 0x1000;
            func_800CF3AC(D_800E213C, &color, particle->timer);
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428)
                palette += 4;
            func_800CEE20((GteShortVector *)particle, 0, scale / 2, scale,
                          (s16)D_800F3368.parameter02 * (s16)(particle->timer / 6) + 0x60,
                          GetClut(0, palette), 1, 0x80, &color);
            break;
        }
        break;
    }
    return 0;
}
