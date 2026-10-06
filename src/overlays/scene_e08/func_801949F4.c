#include "common.h"
#include "pe1/field_glow_sprite.h"

s32 func_800C2B50(void);
void func_800C2EAC(s32);
void func_800C2FF0(s32, s32);
void func_800C3098(s32);
void func_800C3238(s32);
void func_80071A44(GteVector *arg0, s32 arg1, s32 arg2);
void func_800C3134(u8 *table, s32 step, u8 *out);

extern u8 D_80198AD4;
extern u8 D_80199570;
extern u8 D_80199574;
extern u16 D_8019957A;

void func_801949F4(s32 arg0, char *arg1, char *rec) {
    s32 ctx;
    u32 idx;
    char *r;
    GteMatrix buf;
    GteVector vdst;
    GteVector vsrc;

    r = rec;
    ctx = func_800C2B50();
    idx = ((*(u8 *)(rec + 0x22) >> 4) << 1) << 1;
    if (idx >= 0xD) {
        idx = 0xC;
    }
    func_800C2EAC(*(u8 *)(ctx + 0x24));
    func_800C2FF0(0x40, 0x40);
    func_800C3098(0x10);
    func_800C3238(2);
    RotMatrix((GteShortVector *)(r + 0x10), &buf);
    func_80071A44(&vsrc, 0, 0x10);
    vsrc.x = *(s16 *)(r + 0x1C);
    vsrc.y = *(s16 *)(r + 0x1C);
    vsrc.z = 0x1000;
    vdst = vsrc;
    ScaleMatrix(&buf, &vdst);
    func_800C3134(&D_80198AD4, *(s16 *)(arg1 + 2), &D_80199570);
    buf.t[0] = *(s32 *)(r + 0);
    buf.t[1] = *(s32 *)(r + 4);
    buf.t[2] = *(s32 *)(r + 8);
    D_80199574 = idx;
    D_8019957A = *(u16 *)(r + 0x20);
    func_800C42A4((FieldGlowSprite *)&D_80199570, &buf, 1);
}
