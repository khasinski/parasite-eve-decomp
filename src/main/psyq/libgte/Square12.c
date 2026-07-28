#include "pe1/gte.h"

void *Square12(void *input, void *output) {
    gte_ldir123v(input);
    gte_sqr12();
    gte_stmac_return(output);
    return output;
}
