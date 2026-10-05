#include "common.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"

void MulRotMatrix(GteMatrix *matrix);

/* Build, optionally scale/compose, and install a rotation matrix.
 * Matching debt: three pinned word-transfer registers. Matrix reads are C;
 * each GTE control-register transfer uses its own instruction macro. */
void func_800CF658(GteRotation *rotation, s32 *scale, GteMatrix *matrix) {
    GteMatrix local;

    if (matrix == 0) {
        matrix = &local;
    }
    RotMatrixYXZ((GteShortVector *)rotation, matrix);
    if (scale != 0) {
        Gte_ScaleMatrix(matrix, (const GteVector *)scale);
    }
    if (rotation->flags != 0) {
        MulRotMatrix(matrix);
    }
    {
        const GteMatrixWords *words = (const GteMatrixWords *)(matrix);
        register u32 a asm("$12");
        register u32 b asm("$13");
        register u32 c asm("$14");
        a = words->r11_r12;
        b = words->r13_r21;
        gte_ctc2_0(a);
        gte_ctc2_1(b);
        a = words->r22_r23;
        b = words->r31_r32;
        c = words->r33_pad;
        gte_ctc2_2(a);
        gte_ctc2_3(b);
        gte_ctc2_4(c);
    }
}
