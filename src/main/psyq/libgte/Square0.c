#include "pe1/gte.h"

void *Square0(void *input, void *output) {
    gte_ldir123v(input);
    gte_sqr();
    gte_stmac_return(output);
    return output;
}
