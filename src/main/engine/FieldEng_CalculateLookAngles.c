#include "pe1/render_object.h"

void FieldEng_CalculateLookAngles(GteShortVector *from, GteShortVector *to,
                                 GteShortVector *out) {
    int dx, dz, horizontal_distance;

    dz = to->z - from->z;
    dx = to->x - from->x;
    out->y = -ratan2(dz, dx) + 1024;
    horizontal_distance = SquareRoot0((unsigned)dx * dx + (unsigned)dz * dz);
    out->x = -ratan2(to->y - from->y, horizontal_distance);
    out->z = 0;
    out->x &= 4095;
    out->y &= 4095;
}
