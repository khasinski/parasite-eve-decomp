#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"

void FieldEng_TransformTranslation(const GteShortVector *input,
                                  GteMatrixWords *output) {
    GteMatrixWords local;
    /* Retail transfer registers; tracked in crutch debt. */
    register GteMatrixWords *matrix asm("$3");
    s32 **slot = &D_800BCFA4.value;
    register int x asm("$12");
    register int y asm("$13");
    register int z asm("$14");

    /* Preserve the separate address of the current-matrix slot. */
    asm volatile("" : "=r"(slot) : "0"(slot));
    matrix = (GteMatrixWords *)*slot;
    x = matrix->r11_r12;
    y = matrix->r13_r21;
    gte_ctc2_0(x);
    gte_ctc2_1(y);
    x = matrix->r22_r23;
    y = matrix->r31_r32;
    z = matrix->r33_pad;
    gte_ctc2_2(x);
    gte_ctc2_3(y);
    gte_ctc2_4(z);

    x = matrix->tx;
    y = matrix->ty;
    gte_ctc2_5(x);
    z = matrix->tz;
    gte_ctc2_6(y);
    gte_ctc2_7(z);

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

    x = output->tx;
    y = output->ty;
    gte_ctc2_5(x);
    z = output->tz;
    gte_ctc2_6(y);
    gte_ctc2_7(z);
}
