#include "common.h"

extern s16 D_800F3374;
extern u8 D_8019880C[];

void *func_800C2B50(void);
void func_800C2EAC(u8 arg0);
void func_800C3098(s32 arg0);
void func_800C3238(s32 arg0);
void func_800C3134(u8 *table, s32 step, u8 *out);
s32 func_80077A64(s32 a, s32 b, s32 c, s32 d);
s32 func_80077AA4(s32 a, s32 b);
void func_800D3114(void *entry, s32 a1, s32 a2, s32 a3, s32 s10, s32 s14,
                   s32 s18, s32 s1c, s32 s20, s32 s24, u8 *s28, u8 *s2c, s32 s30);

void func_801919B8(s32 arg0, s32 arg1, u8 *base) {
    u8 *st;
    u32 i;
    u8 buf38[8];
    u8 buf40[8];

    st = (u8 *)func_800C2B50();
    func_800C2EAC(st[0x24]);
    func_800C3098(0x10);
    func_800C3238(2);

    for (i = 0; i < 0x10; i++) {
        s16 type = *(s16 *)(base + i * 2 + 0x1C0);
        if (type == 1) {
            s32 v64;
            s32 vAA;
            func_800C3134(D_8019880C, *(s32 *)(base + i * 4 + 0x180), buf38);
            func_800C3134(D_8019880C, *(s32 *)(base + i * 4 + 0x180), buf40);
            buf40[0] = 0;
            buf40[1] = 0;
            buf40[2] = 0;
            D_800F3374 = 0x1F4;
            v64 = func_80077A64(0, 0, 0x340, 0x100) & 0xFFFF;
            vAA = func_80077AA4(0, 0x1D7) & 0xFFFF;
            func_800D3114(base + i * 0x18, 2, *(s16 *)(base + i * 2 + 0x240), 0x40,
                          0x80, 0x9C, 0x10, v64, vAA, *(s16 *)(base + i * 2 + 0x200),
                          buf38, buf40, type);
        }
    }
}
