/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G0 */
#include "pe1/field_glow_layers.h"
#include "pe1/gte.h"

/* Four-layer variant of the field glow: two pairs of fixed spins, the
 * second pair pushed further out and drawn at the full depth.
 * Matching debt: 10 register pins and the remaining empty constraints.
 * Explicit spin address aliases retain the retail spill order. Rotation,
 * column, and translation windows use the stock GTE macros. */
void func_800CAE0C(void *object, void *slot, FieldGlowLayers *glow)
{
    register GteMatrix *workMatrix asm("$16");
    register GteMatrix *stepMatrix asm("$18");
    register const GteMatrixWords *baseRotation asm("$16");
    GteShortVector *spinCAddress;
    GteShortVector *spinDAddress;
    register const GteMatrixWords *baseTranslation asm("$16");
    register u16 *firstColumn asm("$16");
    register const volatile u16 *column1 asm("$19");
    register const volatile u16 *column2 asm("$22");
    register u16 *outColumn1 asm("$20");
    u16 *outColumn2;
    const s32 *translation;
    s32 *outTranslation;
    register GteShortVector *nextSpin asm("$23");
    GteShortVector *thirdSpin;
    GteShortVector *fourthSpin;
    GteMatrix matrix;
    GteMatrix rotation;
    GteShortVector spinA;
    GteShortVector spinB;
    GteShortVector spinC;
    GteShortVector spinD;
    GteVector scaleA;
    GteVector scaleB;
    GteVector scaleC;
    GteVector scaleD;

    spinA = D_800C21D4;
    spinB = D_800C21DC;
    spinCAddress = &spinC;
    spinDAddress = &spinD;
    spinC = D_800C21E4;
    spinD = D_800C21EC;
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x10);
    func_800C3238(2);
    D_800F34D0 = -10;
    D_800F34D2 = (s16)glow->depth >> 1;

    RotMatrixYXZ(&spinA, &rotation);
    stepMatrix = &rotation;
    asm("" : "=r"(stepMatrix) : "0"(stepMatrix));
    matrix = glow->matrix;
    matrix.t[0] = glow->x;
    matrix.t[1] = glow->y;
    matrix.t[2] = glow->z;
    rotation.t[0] = 0;
    rotation.t[1] = 0;
    rotation.t[2] = -20;
    nextSpin = &spinB;
    thirdSpin = spinCAddress;
    fourthSpin = spinDAddress;
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
    asm volatile("" : : : "memory");
    gte_ldlv0(translation);
    gte_rt();
    outTranslation = matrix.t;
    gte_swc2_9_0(outTranslation);
    gte_swc2_10_4(outTranslation);
    gte_swc2_11_8(outTranslation);


    scaleA = D_800C21F4;
    ScaleMatrix(workMatrix, &scaleA);
    func_800C42A4(&D_800F34C8, workMatrix, 0);

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


    scaleB = D_800C21F4;
    ScaleMatrix(workMatrix, &scaleB);
    func_800C42A4(&D_800F34C8, workMatrix, 0);

    D_800F34D0 = -50;
    D_800F34D2 = glow->depth;

    RotMatrixYXZ(thirdSpin, stepMatrix);
    matrix = glow->matrix;
    matrix.t[0] = glow->x;
    matrix.t[1] = glow->y;
    matrix.t[2] = glow->z;
    rotation.t[0] = 0;
    rotation.t[1] = -10;
    rotation.t[2] = -400;
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


    scaleC = D_800C21F4;
    ScaleMatrix(workMatrix, &scaleC);
    func_800C42A4(&D_800F34C8, workMatrix, 0);

    RotMatrixYXZ(fourthSpin, stepMatrix);
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


        asm("" : : "r"(outColumn2));
    scaleD = D_800C21F4;
    ScaleMatrix(workMatrix, &scaleD);
    func_800C42A4(&D_800F34C8, workMatrix, 0);
}
