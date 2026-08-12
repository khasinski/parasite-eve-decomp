#include "pe1/geom_state.h"
#include "pe1/gte.h"
/* MASPSX_FLAGS: --expand-div */

int Geo_ClipAlternatingArcPoint(int radius, int divisor, int segment) {
    int angle = (segment << 10) / divisor;
    int direction = ((segment & 1) << 1) - 1;
    int product = rcos(angle) * radius;
    int y;
    GeomState *state;

    if (product < 0) {
        product += 0xFFF;
    }
    product >>= 12;
    y = product * direction;

    state = D_800B1624;
    state->clip_min_x = state->clip_min_y = -128;
    state = D_800B1624;
    state->clip_max_x = state->clip_max_y = 128;

    return Geo_ClipPoint(0, (s16)y, 0);
}
