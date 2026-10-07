/* Field glow layers: compose the effect's axis matrix with two fixed spins
 * and draw the sprite once per spin, scaled by a fixed vector.
 * FIELD_GLOW_LAYERS_FUNC    entry point
 * FIELD_GLOW_LAYERS_SPIN_A  GteShortVector first spin
 * FIELD_GLOW_LAYERS_SPIN_B  GteShortVector second spin
 * FIELD_GLOW_LAYERS_SCALE   GteVector sprite scale
 * FIELD_GLOW_LAYERS_SPRITE  FieldGlowSprite parameter block */
#include "pe1/field_glow_layers.h"
#include "pe1/gte.h"

/* Matching debt: 9 register pins and 12 empty constraints.
 * Matrix arithmetic and loads are C; GTE instructions are wrapped separately.
 * Volatile column reads retain each rotation load instead of caching values
 * across the two compositions. */
void FIELD_GLOW_LAYERS_FUNC(void *object, void *slot, FieldGlowLayers *glow)
{
    register GteMatrix *workMatrix asm("$16");
    register GteMatrix *stepMatrix asm("$18");
    register const GteMatrixWords *baseRotation asm("$16");
    register const GteMatrixWords *baseTranslation asm("$16");
    register u16 *firstColumn asm("$16");
    register const volatile u16 *column1 asm("$19");
    const volatile u16 *column2;
    register u16 *outColumn1 asm("$20");
    register u16 *outColumn2 asm("$23");
    const s32 *translation;
    register s32 *outTranslation asm("$21");
    GteShortVector *nextSpin;
    GteMatrix matrix;
    GteMatrix rotation;
    GteShortVector spinA;
    GteShortVector spinB;
    GteVector scaleA;
    GteVector scaleB;

    spinA = FIELD_GLOW_LAYERS_SPIN_A;
    spinB = FIELD_GLOW_LAYERS_SPIN_B;
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x10);
    func_800C3238(2);
    FIELD_GLOW_LAYERS_SPRITE.depth = glow->depth;

    RotMatrixYXZ(&spinA, &rotation);
    stepMatrix = &rotation;
    asm("" : "=r"(stepMatrix) : "0"(stepMatrix));
    matrix = glow->matrix;
    matrix.t[0] = glow->x;
    matrix.t[1] = glow->y;
    matrix.t[2] = glow->z;
    nextSpin = &spinB;
    rotation.t[0] = 0;
    rotation.t[1] = 0;
    rotation.t[2] = 0;
    asm volatile("" : : : "memory");
    workMatrix = &matrix;
    asm("" : "=r"(workMatrix) : "0"(workMatrix));
    baseRotation = (const GteMatrixWords *)workMatrix;

    gte_ldrotmatrix(baseRotation);
    gte_ldclmv((const u16 *)stepMatrix);
    gte_rtir();
    firstColumn = (u16 *)workMatrix;

    gte_stclmv(firstColumn);
    column1 = (const volatile u16 *)&rotation + 1;
    asm("" : "=r"(column1) : "0"(column1));
    gte_ldclmv(column1);
    gte_rtir();
    outColumn1 = (u16 *)&matrix + 1;
    asm("" : "=r"(outColumn1) : "0"(outColumn1));
    gte_stclmv(outColumn1);
    column2 = (const volatile u16 *)&rotation + 2;
    asm("" : "=r"(column2) : "0"(column2));
    gte_ldclmv(column2);
    gte_rtir();
    outColumn2 = (u16 *)&matrix + 2;
    asm("" : "=r"(outColumn2) : "0"(outColumn2));
    gte_stclmv(outColumn2);
    /* Compose the translation using the current matrix. */
    asm volatile("" : : : "memory");
    baseTranslation = (const GteMatrixWords *)workMatrix;

    gte_ldtransmatrix(baseTranslation);
    translation = rotation.t;
    asm("" : "=r"(translation) : "0"(translation));
    gte_ldlv0(translation);
    gte_rt();
    outTranslation = matrix.t;
    gte_swc2_9_0(outTranslation);
    gte_swc2_10_4(outTranslation);
    gte_swc2_11_8(outTranslation);

    scaleA = FIELD_GLOW_LAYERS_SCALE;
    ScaleMatrix(workMatrix, &scaleA);
    func_800C42A4(&FIELD_GLOW_LAYERS_SPRITE, workMatrix, 0);

    RotMatrixYXZ(nextSpin, stepMatrix);
    matrix = glow->matrix;
    matrix.t[0] = glow->x;
    matrix.t[1] = glow->y;
    matrix.t[2] = glow->z;
    asm volatile("" : : : "memory");
    baseRotation = (const GteMatrixWords *)workMatrix;

    gte_ldrotmatrix(baseRotation);
    gte_ldclmv((const u16 *)stepMatrix);
    gte_rtir();
    firstColumn = (u16 *)workMatrix;

    gte_stclmv(firstColumn);
    gte_ldclmv(column1);
    gte_rtir();
    gte_stclmv(outColumn1);
    gte_ldclmv(column2);
    gte_rtir();
    gte_stclmv(outColumn2);
    /* Compose the translation using the current matrix. */
    asm volatile("" : : : "memory");
    baseTranslation = (const GteMatrixWords *)workMatrix;

    gte_ldtransmatrix(baseTranslation);
    gte_ldlv0(translation);
    gte_rt();
    gte_swc2_9_0(outTranslation);
    gte_swc2_10_4(outTranslation);
    gte_swc2_11_8(outTranslation);

    asm("" : : "r"(glow), "r"(glow), "r"(glow), "r"(glow), "r"(glow), "r"(glow), "r"(glow));
    scaleB = FIELD_GLOW_LAYERS_SCALE;
    ScaleMatrix(workMatrix, &scaleB);
    func_800C42A4(&FIELD_GLOW_LAYERS_SPRITE, workMatrix, 0);
}

#undef FIELD_GLOW_LAYERS_FUNC
#undef FIELD_GLOW_LAYERS_SPIN_A
#undef FIELD_GLOW_LAYERS_SPIN_B
#undef FIELD_GLOW_LAYERS_SCALE
#undef FIELD_GLOW_LAYERS_SPRITE
