#include "pe1/gte_types.h"

GteMatrix *TransMatrix(GteMatrix *matrix, const int *v) {
    register GteMatrix *m asm("$4") = matrix;
    register int x asm("$8") = v[0];
    register int y asm("$9") = v[1];
    register int z asm("$10") = v[2];
    GteMatrix *ret;
    m->t[0] = x;
    m->t[1] = y;
    m->t[2] = z;

    /* Finish the translation stores before copying the matrix pointer. */
    asm volatile("" : "=r"(m) : "0"(m), "m"(m->t[0]), "m"(m->t[1]), "m"(m->t[2]));
    ret = m;
    /* Keep the pointer copy ahead of the return branch. */
    asm volatile("" : "=r"(ret) : "0"(ret));
    return ret;
}
