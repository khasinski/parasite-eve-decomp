#include "pe1/gte.h"

void LoadAverageShort0(void *first, void *second, int first_scale,
                       int second_scale, void * volatile output) {
    gte_load_average_short0(first, second, first_scale, second_scale, output);
}
