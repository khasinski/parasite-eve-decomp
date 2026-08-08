#include "common.h"
/* CC1_FLAGS: -fno-strength-reduce */
#include "pe1/room_fx.h"

extern s32 *func_800C2B50(void);
extern s32 *func_800C2B10(s32 index);
extern s32 *func_800C2B28(s32 index);
extern void func_800C6800(s32 arg0, s32 arg1, void *arg2);

void func_80191EA0(s32 arg0, s32 arg1, char *state) {
    s32 *src;
    u32 i;
    s32 neg_y;
    s32 scale;
    s32 one;
    char *flags;
    char *shorts;
    char *vecs;
    s32 y;
    s32 height;

    src = func_800C2B50();
    *(RoomFxVec4 *)(state + 0x160) = *(RoomFxVec4 *)(src + 1);
    *(RoomFxVec4 *)(state + 0x170) = *(RoomFxVec4 *)(src + 5);

    *(s16 *)(state + 0x180) = *func_800C2B10(1);
    *(s16 *)(state + 0x182) = *func_800C2B28(8);
    *(s16 *)(state + 0x184) = 0;

    i = 0;
    neg_y = -0x12C;
    scale = 0x800;
    one = 1;
    flags = state;
    height = 0x60;
    shorts = state;
    y = 0;
    vecs = state;
    while (i < 16) {
        *(s16 *)(vecs + 0x60) = 0;
        *(s16 *)(vecs + 0x62) = neg_y;
        *(s16 *)(vecs + 0x64) = y;
        *(s16 *)(vecs + 0xE0) = 0;
        *(s16 *)(vecs + 0xE2) = 0;
        *(s16 *)(vecs + 0xE4) = 0;
        *(s16 *)(shorts + 0x40) = scale;
        *(s16 *)(shorts + 0x20) = height;
        *flags = one;
        flags++;
        height -= 3;
        shorts += 2;
        y += 0x78;
        i++;
        vecs += 8;
    }

    func_800C6800(arg0, 0x56D, state + 0x60);
}
