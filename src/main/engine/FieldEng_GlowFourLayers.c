/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G0 */
#include "pe1/field_glow_layers.h"
#include "pe1/gte.h"

/* Four-layer variant of the field glow: two pairs of fixed spins, the
 * second pair pushed further out and drawn at the full depth. */
void func_800CAE0C(void *object, void *slot, FieldGlowLayers *glow)
{
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
    spinC = D_800C21E4;
    spinD = D_800C21EC;
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x10);
    func_800C3238(2);
    D_800F34D0 = -10;
    D_800F34D2 = (s16)glow->depth >> 1;

    RotMatrixYXZ(&spinA, &rotation);
    matrix = glow->matrix;
    matrix.t[0] = glow->x;
    matrix.t[1] = glow->y;
    matrix.t[2] = glow->z;
    rotation.t[0] = 0;
    rotation.t[1] = 0;
    rotation.t[2] = -20;
    gte_CompMatrix(&matrix, &rotation, &matrix);
    scaleA = D_800C21F4;
    Gte_ScaleMatrix(&matrix, &scaleA);
    func_800C42A4(&D_800F34C8, &matrix, 0);

    RotMatrixYXZ(&spinB, &rotation);
    matrix = glow->matrix;
    matrix.t[0] = glow->x;
    matrix.t[1] = glow->y;
    matrix.t[2] = glow->z;
    gte_CompMatrix(&matrix, &rotation, &matrix);
    scaleB = D_800C21F4;
    Gte_ScaleMatrix(&matrix, &scaleB);
    func_800C42A4(&D_800F34C8, &matrix, 0);

    D_800F34D0 = -50;
    D_800F34D2 = glow->depth;

    RotMatrixYXZ(&spinC, &rotation);
    matrix = glow->matrix;
    matrix.t[0] = glow->x;
    matrix.t[1] = glow->y;
    matrix.t[2] = glow->z;
    rotation.t[0] = 0;
    rotation.t[1] = -10;
    rotation.t[2] = -400;
    gte_CompMatrix(&matrix, &rotation, &matrix);
    scaleC = D_800C21F4;
    Gte_ScaleMatrix(&matrix, &scaleC);
    func_800C42A4(&D_800F34C8, &matrix, 0);

    RotMatrixYXZ(&spinD, &rotation);
    matrix = glow->matrix;
    matrix.t[0] = glow->x;
    matrix.t[1] = glow->y;
    matrix.t[2] = glow->z;
    gte_CompMatrix(&matrix, &rotation, &matrix);
    /* Matching debt: retain this column address through the fourth compose.
     * This empty barrier reproduces retail's saved-register/spill allocation. */
    asm("" : : "r"(&rotation.m[0][1]));
    scaleD = D_800C21F4;
    Gte_ScaleMatrix(&matrix, &scaleD);
    func_800C42A4(&D_800F34C8, &matrix, 0);
}
