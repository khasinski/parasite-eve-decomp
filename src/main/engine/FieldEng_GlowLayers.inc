/* Field glow layers: compose the effect's axis matrix with two fixed spins
 * and draw the sprite once per spin, scaled by a fixed vector.
 * FIELD_GLOW_LAYERS_FUNC    entry point
 * FIELD_GLOW_LAYERS_SPIN_A  GteShortVector first spin
 * FIELD_GLOW_LAYERS_SPIN_B  GteShortVector second spin
 * FIELD_GLOW_LAYERS_SCALE   GteVector sprite scale
 * FIELD_GLOW_LAYERS_SPRITE  FieldGlowSprite parameter block */
#include "pe1/field_glow_layers.h"
#include "pe1/gte.h"

void FIELD_GLOW_LAYERS_FUNC(void *object, void *slot, FieldGlowLayers *glow)
{
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
    matrix = glow->matrix;
    matrix.t[0] = glow->x;
    matrix.t[1] = glow->y;
    matrix.t[2] = glow->z;
    rotation.t[0] = 0;
    rotation.t[1] = 0;
    rotation.t[2] = 0;
    gte_CompMatrix(&matrix, &rotation, &matrix);
    scaleA = FIELD_GLOW_LAYERS_SCALE;
    Gte_ScaleMatrix(&matrix, &scaleA);
    func_800C42A4(&FIELD_GLOW_LAYERS_SPRITE, &matrix, 0);

    RotMatrixYXZ(&spinB, &rotation);
    matrix = glow->matrix;
    matrix.t[0] = glow->x;
    matrix.t[1] = glow->y;
    matrix.t[2] = glow->z;
    gte_CompMatrix(&matrix, &rotation, &matrix);
    scaleB = FIELD_GLOW_LAYERS_SCALE;
    Gte_ScaleMatrix(&matrix, &scaleB);
    func_800C42A4(&FIELD_GLOW_LAYERS_SPRITE, &matrix, 0);
}

#undef FIELD_GLOW_LAYERS_FUNC
#undef FIELD_GLOW_LAYERS_SPIN_A
#undef FIELD_GLOW_LAYERS_SPIN_B
#undef FIELD_GLOW_LAYERS_SCALE
#undef FIELD_GLOW_LAYERS_SPRITE
