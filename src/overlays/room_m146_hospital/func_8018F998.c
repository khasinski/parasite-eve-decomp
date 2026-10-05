#include "pe1/gte_types.h"

typedef GteMatrix Room146Matrix;
typedef GteVector Room146Vec4;

extern Room146Vec4 D_8018EFFC;
extern s16 g_FrameCount16;
extern char D_80192668;

char *func_800C2B50(void);
void func_80078CC4(Room146Matrix *matrix, Room146Vec4 *scale);
void func_800C2EAC(u8 arg0);
void func_800C2FF0(s32 arg0, s32 arg1);
void func_800C3098(s32 arg0);
void func_800C3238(s32 arg0);
void func_800C42A4(void *arg0, Room146Matrix *matrix, s32 arg2);

void func_8018F998(void) {
    Room146Matrix matrix;
    Room146Vec4 scale;
    char *owner;

    owner = func_800C2B50();
    func_800C2EAC(owner[0x24]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;
    scale = D_8018EFFC;
    func_80078CC4(&matrix, &scale);

    matrix.t[0] = *(s32 *)(owner + 0x18);
    matrix.t[1] = g_FrameCount16;
    matrix.t[2] = *(s32 *)(owner + 0x20);
    func_800C42A4(&D_80192668, &matrix, 1);
}
