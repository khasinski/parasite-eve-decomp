#include "pe1/render_object.h"
#include "pe1/random.h"
#include "pe1/battle_runtime.h"
int func_800DD19C(int mode, RenderSparkEmitter *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color;
    int scale;
    switch (mode) {
    case 0:
        state->phase = rand();
        func_800CE870((char *)D_8009D254, 1, &state->position.x);
        state->position.y -= 510;
        return 0;
    case 1:
        if (D_800E27EC >= 64) return 1;
        break;
    case 2:
        D_800F3368.depth = 60;
        position.x = state->position.x;
        position.y = state->position.y;
        position.z = state->position.z;
        scale = 4096 + (D_800E27EC << 5);
        func_800CF3AC(D_800E1FA4, &color, D_800E27EC);
        func_800D004C(&position, 500, 500, 16, 0, scale, scale, &color, 0, 128, 1);
        func_800D0728(&position, 410, 500, 20, 0, 4096, 4096, 0, &color, 128, 3);
        func_800CF3AC(D_800E1FCC, &color, D_800E27EC);
        rotation.x = 0;
        rotation.y = 0;
        rotation.flags = 0;
        rotation.z = D_800E27EC * 16;
        func_800D004C(&position, 400, 100, 24, &rotation, scale, scale, &color, 0, 128, 1);
        break;
    }
    return 0;
}
