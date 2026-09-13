#include "pe1/render_object.h"

void FieldEng_CalculateLookAngles(GteShortVector *from, GteShortVector *to,
                                 GteShortVector *out) {
    int dx, dz, horizontal_distance;

    dz = to->z - from->z;
    dx = to->x - from->x;
    out->y = -Gte_Atan2(dz, dx) + 1024;
    horizontal_distance = Gte_ISqrt((unsigned)dx * dx + (unsigned)dz * dz);
    out->x = -Gte_Atan2(to->y - from->y, horizontal_distance);
    out->z = 0;
    out->x &= 4095;
    out->y &= 4095;
}
