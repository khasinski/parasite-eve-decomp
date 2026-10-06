#include "common.h"
#include "pe1/field_sprite_state.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/gte_types.h"

void *memset(void *dest, int value, unsigned int count);

extern GteShortVector D_800C223C;
extern s16 D_800E22A2;

int func_800CD2EC(void *arg0, void *arg1, u8 *anim) {
    s16 *field = &D_800E22A2;
    GteMatrix matrix;
    GteShortVector rot;
    GteVector scaleCopy;
    GteVector scale;

    rot = D_800C223C;

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    *field = *(u16 *)(anim + 0x4);
    RotMatrix(&rot, &matrix);

    matrix.t[0] = *(s16 *)(anim + 0x8);
    matrix.t[1] = *(s16 *)(anim + 0xA);
    matrix.t[2] = *(s16 *)(anim + 0xC);

    memset(&scale, 0, sizeof(scale));
    scale.x = *(s16 *)(anim + 0x6);
    scale.y = *(s16 *)(anim + 0x6);
    scale.z = *(s16 *)(anim + 0x6);
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    func_800C42A4((FieldGlowSprite *)((u8 *)field - 10), &matrix, 0);
}
