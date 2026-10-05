#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"

/* Transform input by the current view matrix, then load the result into
 * the GTE translation registers, optionally returning it in output.
 * Matching debt: seven pins and two empty pointer constraints. Matrix
 * reads are C; every COP2 transfer/command and hazard nop is separate. */
void FieldEng_TransformTranslation(const GteShortVector *input,
                                  GteMatrixWords *output) {
    GteMatrixWords local;
    /* Retail keeps the matrix pointer in v1; tracked in crutch debt. */
    register GteMatrixWords *matrix asm("$3");
    s32 **slot = &D_800BCFA4.value;

    /* Preserve the separate address of the current-matrix slot. */
    asm volatile("" : "=r"(slot) : "0"(slot));
    matrix = (GteMatrixWords *)*slot;
    asm("" : : "r"(matrix));
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
        a = words->tx;
        b = words->ty;
        gte_ctc2_5(a);
        c = words->tz;
        gte_ctc2_6(b);
        gte_ctc2_7(c);
    }

    gte_lwc2_0_0(input);
    gte_lwc2_1_4(input);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();

    if (!output)
        output = &local;
    gte_swc2_25_0(&output->tx);
    gte_swc2_26_4(&output->tx);
    gte_swc2_27_8(&output->tx);

    {
        const GteMatrixWords *words = (const GteMatrixWords *)(output);
        register u32 a asm("$12");
        register u32 b asm("$13");
        register u32 c asm("$14");
        a = words->tx;
        b = words->ty;
        gte_ctc2_5(a);
        c = words->tz;
        gte_ctc2_6(b);
        gte_ctc2_7(c);
    }
}
