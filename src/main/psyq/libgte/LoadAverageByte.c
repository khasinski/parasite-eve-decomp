#include "pe1/gte.h"

void LoadAverageByte(void *first, void *second, int first_scale,
                     int second_scale, void * volatile output) {
    gte_load_average_byte2(first, second, first_scale, second_scale, output);
}
