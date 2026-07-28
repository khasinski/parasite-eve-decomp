#include "pe1/gte.h"

void LightColor(void *input, void *output) {
    gte_ldir123v(input);
    gte_lightcolor();
    gte_stir123(output);
}
