#include "pe1/render_object.h"
#include "pe1/battle_runtime.h"
#include "pe1/gte.h"

int FieldEng_RotatingEffect(int mode) {
    GteShortVector position;
    GteRotation rotation;
    u32 color;
    int scale;

    switch (mode) {
    case 0:
        break;
    case 1:
        if (D_800E27EC >= 24)
            return 1;
        break;
    case 2:
        D_800F3374 = 0;
        func_800CE870((char *)D_8009D254, 1, &position.x);
        position.y -= 300;
        func_800CF3AC(D_800E1494, &color, D_800E27EC);
        rotation.x = 1024;
        rotation.y = 0;
        rotation.z = D_800E27EC << 8;
        rotation.flags = 1;
        scale = rsin((D_800E27EC << 10) / 24);
        func_800D0728(&position, 300, 400, 10, &rotation,
                      scale, scale, 0, &color, 128, 1);
        break;
    }
    return 0;
}
