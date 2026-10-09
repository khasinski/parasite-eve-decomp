#include "common.h"
#include "pe1/field_sprite_state.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/gte_types.h"
#include "pe1/field_anim_particle.h"

void *memset(void *dest, int value, unsigned int count);

extern GteShortVector D_800C223C;

int func_800CD2EC(void *arg0, void *arg1, FieldAnimGlowPoint *anim) {
    s16 *field = (s16 *)&D_800E2298.depth;
    GteMatrix matrix;
    GteShortVector rot;
    GteVector scaleCopy;
    GteVector scale;

    rot = D_800C223C;

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    *field = anim->brightness;
    RotMatrix(&rot, &matrix);

    matrix.t[0] = (s16)anim->position.x;
    matrix.t[1] = (s16)anim->position.y;
    matrix.t[2] = (s16)anim->position.z;

    memset(&scale, 0, sizeof(scale));
    scale.x = anim->scale;
    scale.y = anim->scale;
    scale.z = anim->scale;
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    func_800C42A4((FieldGlowSprite *)((u8 *)field - PE1_OFFSETOF(FieldGlowSprite, depth)), &matrix, 0);
}
