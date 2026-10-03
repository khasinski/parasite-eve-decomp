#include "pe1/render_object.h"
#include "pe1/field_anim.h"
#include "pe1/random.h"
#include "pe1/field_actor.h"
#include "pe1/gte.h"

/* Ring pulse at the actor target: a breathing disc for 32 frames and an
 * expanding ground ring over the first 17. */
int func_800D6A1C(int mode, RenderRingPulse *state)
{
    GteRotation rotation;
    RenderColor color = D_800C22CC;
    int scale;
    int intensity;

    switch (mode) {
    case 0:
        state->phase = rand();
        state->position.x = D_800F32D0->actor->render_object.target_x;
        state->position.y = D_800F32D0->actor->render_object.target_y;
        state->position.z = D_800F32D0->actor->render_object.target_z;
        state->raised.x = state->position.x;
        state->raised.y = state->position.y;
        state->raised.z = state->position.z;
        state->raised.y -= 0x1B8;
        return 0;
    case 1:
        if (D_800E27EC >= 32)
            return 1;
        break;
    case 2:
        D_800F3368.depth = 100;
        scale = rcos(D_800E27EC << 5) / 2 + 0x800;
        intensity = rcos(D_800E27EC << 5) / 32;
        func_800D004C(&state->position, 700, 700, 12, 0, scale, scale,
                      &color, 0, intensity, 1);
        D_800F3368.depth = 0;
        if (D_800E27EC < 17) {
            intensity = 128 - (D_800E27EC << 3);
            rotation.x = 0x400;
            rotation.y = 0;
            rotation.z = D_800E27EC << 7;
            rotation.flags = 1;
            scale = rsin(D_800E27EC << 6) * 2;
            func_800D0728(&state->position, 800, 1000, 16, &rotation, scale,
                          scale, 0, &color, intensity, 1);
        }
        break;
    }
    return 0;
}
