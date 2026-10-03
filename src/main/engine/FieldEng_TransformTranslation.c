#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"

void FieldEng_TransformTranslation(const GteShortVector *input,
                                  GteMatrixWords *output) {
    GteMatrixWords local;
    /* Retail keeps the matrix pointer in v1; tracked in crutch debt. */
    register GteMatrixWords *matrix asm("$3");
    s32 **slot = &D_800BCFA4.value;

    /* Preserve the separate address of the current-matrix slot. */
    asm volatile("" : "=r"(slot) : "0"(slot));
    matrix = (GteMatrixWords *)*slot;
    gte_ldrotmatrix(matrix);
    gte_ldtransmatrix(matrix);

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

    gte_ldtransmatrix(output);
}
