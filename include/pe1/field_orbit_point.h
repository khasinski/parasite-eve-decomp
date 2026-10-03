#ifndef PE1_FIELD_ORBIT_POINT_H
#define PE1_FIELD_ORBIT_POINT_H

#include "pe1/gte_types.h"

/* Identity rotation with zero translation used as the roll matrix seed. */
extern GteMatrix D_800C2270;

void MulRotMatrix(GteMatrix *matrix);

#endif /* PE1_FIELD_ORBIT_POINT_H */
