#include "common.h"
#include "pe1/gte_types.h"

void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C42A4(void *arg0, void *arg1, int arg2);
void *memset(void *dest, int value, unsigned int count);

extern GteShortVector D_800C223C;
extern u8 D_800E2298;
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

    Gte_ScaleMatrix(&matrix, &scaleCopy);
    func_800C42A4((u8 *)field - 10, &matrix, 0);
}
