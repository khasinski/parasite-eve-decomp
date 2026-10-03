#include "pe1/render_object.h"
#include "pe1/gte.h"
#include "pe1/battle_runtime.h"

typedef struct RoomM318Node {
    char pad[8];
    void *link;
} RoomM318Node;

extern int D_801995B8;
extern char D_801995BC[], D_801995DC[];
extern RoomM318Node *D_800F32D0;

int func_80193D88(int mode, RenderSparkEmitter *state)
{
    GteShortVector position;
    RenderColor color;
    int scale;
    switch (mode) {
    case 0:
        state->phase = D_801995B8;
        D_801995B8 += 1365;
        func_800CE870(D_800F32D0->link, 0, &state->position.x);
        state->position.y -= 100;
        state->position.x += rcos(state->phase) * 300 / 4096;
        state->position.z += rsin(state->phase) * 300 / 4096;
        return 0;
    case 1:
        if (D_800E27EC >= 8) return 1;
        break;
    case 2:
        D_800F3368.depth = 60;
        position.x = state->position.x;
        position.y = state->position.y;
        position.z = state->position.z;
        scale = 2048 + (D_800E27EC << 9);
        func_800CF3AC(D_801995BC, &color, D_800E27EC);
        func_800D004C(&position, 500, 500, 16, 0, scale, scale, &color, 0, 128, 1);
        func_800D0728(&position, 410, 500, 20, 0, 4096, 4096, 0, &color, 128, 3);
        func_800CF3AC(D_801995DC, &color, D_800E27EC);
        func_800D004C(&position, 500, 100, 16, 0, scale, scale, &color, 0, 128, 1);
        break;
    }
    return 0;
}
