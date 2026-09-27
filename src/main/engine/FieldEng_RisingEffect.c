#include "pe1/render_object.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"
#include "pe1/random.h"

static inline int TextureIndex(unsigned int factor, unsigned int step)
{
    return factor * step + 128;
}

int func_800D5CE4(int mode, RenderRisingEffect *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color;
    int scale;
    u16 clut;
    int y;
    int random;
    u16 velocity;
    unsigned int frame;
    int phase;
    switch (mode) {
    case 1:
        state->x += state->vx;
        state->y += state->vy;
        state->z += state->vz;
        random = rand();
        velocity = state->vy + 2;
        state->vy = velocity + (random & 1);
        if (D_800E27EC >= 24)
            return 1;
        break;
    case 2:
        func_800CF3AC(D_800E1694, &color, D_800E27EC);
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        frame = D_800E27EC;
        rotation.z = state->angle + (frame << 5);
        phase = frame << 10;
        scale = rsin(phase / 24) + 0x1000;
        y = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428)
            y += 4;
        clut = GetClut(32, y);
        func_800CEE20(&position, &rotation, scale, scale,
            TextureIndex(D_800F336A, D_800E27EC / 3),
            clut, 1, 128, &color);
        break;
    }
    return 0;
}
