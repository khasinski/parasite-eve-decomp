#include "common.h"
/* CC1_FLAGS: -fno-strength-reduce */
void func_800C2EAC(int arg0);
void func_800C3098(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3238(int arg0);
void func_800C3B04(void *arg0);

typedef struct FieldAnimPointTriple {
    u16 x;
    u16 y;
    u16 z;
} FieldAnimPointTriple;

extern short D_800E2840;

void FieldAnim_ProcessPointTriples(void *arg0, void *arg1, u8 *anim) {
    volatile int stack_pad;
    u16 *entry;
    FieldAnimPointTriple *out;
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
        out = (FieldAnimPointTriple *)((u8 *)base - 0x28);
        entry = (u16 *)anim;
        /* This one-shot block preserves retail GCC's s1/s2 allocation. */
        do {
            do {
                out->x = entry[4];
                out->y = entry[0x14];
                out->z = entry[0x24];
                func_800C3B04(out);
                i++;
                entry++;
            } while (i < *(short *)(anim + 0x4));
        } while (0);
    }
}
