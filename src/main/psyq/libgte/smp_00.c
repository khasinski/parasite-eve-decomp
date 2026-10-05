#include "pe1/gte.h"

void LightColor(void *input, void *output) {
    gte_ldir123v(input);
    gte_lightcolor();
    gte_stir123(output);
}

void DpqColorLight(void *input, void *color, int depth, void *output) {
    gte_ldir123v(input);
    gte_ldrgb(color);
    gte_ldir0(depth);
    gte_dpcl();
    gte_strgb(output);
}

void DpqColor3(void *rgb0, void *rgb1, void *rgb2, int interpolation,
               void * volatile out0, void * volatile out1,
               void * volatile out2) {
    gte_declare_three_outputs();

    gte_ldrgb0(rgb0);
    gte_ldrgb1(rgb1);
    gte_ldrgb2(rgb2);
    gte_ldrgb(rgb2);
    gte_ldir0(interpolation);
    gte_dpct_separate();
    gte_bind_three_outputs(out0, out1, out2);
    gte_strgb0_bound();
    gte_strgb1_bound();
    gte_strgb2_bound();
}

void Intpl(void *input, int depth, void *output) {
    gte_ldir123v(input);
    gte_ldir0(depth);
    gte_intpl();
    gte_strgb(output);
}

void *Square12(void *input, void *output) {
    gte_ldir123v(input);
    gte_sqr12();
    gte_stmac_return(output);
    return output;
}

void *Square0(void *input, void *output) {
    gte_ldir123v(input);
    gte_sqr();
    gte_stmac_return(output);
    return output;
}

int AverageZ3(int z1, int z2, int z3) {
    int result;

    gte_ldsz3(z1, z2, z3);
    gte_avsz3();
    gte_getotz(result);
    return result;
}

int AverageZ4(int z0, int z1, int z2, int z3) {
    int result;

    gte_ldsz4(z0, z1, z2, z3);
    gte_avsz4();
    gte_getotz(result);
    return result;
}

void OuterProduct12(void *matrix_column, void *vector, void *output) {
    gte_pushrotcol0();
    gte_ldopv1(matrix_column);
    gte_ldopv(vector);
    gte_op12();
    gte_stmac(output);
    gte_poprotcol0();
}

void OuterProduct0(void *matrix_column, void *vector, void *output) {
    gte_pushrotcol0();
    gte_ldopv1(matrix_column);
    gte_ldopv(vector);
    gte_op0();
    gte_stmac(output);
    gte_poprotcol0();
}

int Lzc(int value) {
    int result;

    gte_ldlzcs(value);
    gte_getlzcr(result);
    return result;
}
