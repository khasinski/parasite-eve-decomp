#include "pe1/gte.h"

void OuterProduct0(void *matrix_column, void *vector, void *output) {
    gte_pushrotcol0();
    gte_ldopv1(matrix_column);
    gte_ldopv(vector);
    gte_op0();
    gte_stmac(output);
    gte_poprotcol0();
}
