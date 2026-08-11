#include "common.h"
/* CC1_FLAGS: -fno-strength-reduce */
void func_800C2EAC(int arg0);
void func_800C3098(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3238(int arg0);
void func_800C3B04(void *arg0);

extern short D_800E2840;

void FieldAnim_ProcessPointTriples(void *arg0, void *arg1, u8 *anim) {
    volatile int stack_pad;
    u16 *entry;
    void *out;
    short *base;
    unsigned int i;

    i = 0;
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x10, 0x10);
    func_800C3238(0);

    base = &D_800E2840;
    *base = (s32)(anim[3] << 24) >> 23;

    if (*(short *)(anim + 0x4) != 0) {
        out = (u8 *)base - 0x28;
        entry = (u16 *)anim;
        /* This one-shot block preserves retail GCC's s1/s2 allocation. */
        do {
            do {
                ((u16 *)out)[0] = entry[4];
                ((u16 *)out)[1] = entry[0x14];
                ((u16 *)out)[2] = entry[0x24];
                func_800C3B04(out);
                i++;
                entry++;
            } while (i < *(short *)(anim + 0x4));
        } while (0);
    }
}
