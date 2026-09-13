/* MASPSX_FLAGS: --expand-div */
#include "pe1/render_object.h"
#include "pe1/gte.h"

int FieldEng_CosineEffect(int mode, RenderCosineEffect *effect) {
    u32 color;

    switch (mode) {
    case 1:
        effect->x = (effect->amplitude
            * rcos((D_800E27EC << 10) / effect->duration)) / 4096;
        effect->y += 16;
        if (D_800E27EC >= effect->duration)
            return 1;
        break;
    case 2:
        func_800CF3AC(D_800E1C04, &color, D_800E27EC);
        func_800D27FC(effect->x, effect->y, &color, 128, 1);
        break;
    }
    return 0;
}
