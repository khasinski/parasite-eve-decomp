#include "pe1/render_object.h"
#include "pe1/field_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/gte.h"

int func_800D4EA4(int mode)
{
    GteShortVector position;
    RenderColor color;
    u8 *track;
    int scale;

    switch (mode) {
    case 0:
        break;
    case 1:
        if (D_800E27EC >= 20)
            return 1;
        break;
    case 2:
        D_800F3368.depth = 200;
        func_800CE870((char *)D_8009D254, 1, &position.x);
        position.y -= 300;
        track = D_800E2368->variables[6] == 1 ? D_800E1518 : D_800E1540;
        func_800CF3AC(track, &color, D_800E27EC * 24 / 20);
        scale = rsin((D_800E27EC << 10) / 20) / 2 + 2048;
        func_800D004C(&position, 400, 400, 8, 0, scale, scale, &color, 0, 128, 1);
        break;
    }
    return 0;
}
