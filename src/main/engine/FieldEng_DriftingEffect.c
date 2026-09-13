#include "pe1/render_object.h"
#include "pe1/random.h"
#include "pe1/psyq_gpu.h"

int FieldEng_DriftingEffect(int mode, GteShortVector *position) {
    RenderColor color;
    GteRotation rotation;
    int scale;
    int palette;
    switch (mode) {
    case 1:
        position->x += (rand() & 7) - 3;
        position->z += (rand() & 7) - 3;
        position->y -= (rand() & 3) + 2;
        if (D_800E27EC >= 32)
            return 1;
        break;
    case 2:
        rotation.x = 0;
        rotation.y = 0;
        rotation.flags = 0;
        rotation.z = D_800E27EC * 24;
        func_800CF3AC(D_800E1EE8, &color, D_800E27EC);
        scale = (D_800E27EC << 7) + 8192;
        palette = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(position, &rotation, scale, scale, 130,
                      GetClut(48, palette), 1, 128, &color);
        break;
    }
    return 0;
}
