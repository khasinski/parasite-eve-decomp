#include "common.h"
extern volatile u16 D_800E2290;
extern volatile u16 D_800E2292;
extern volatile u16 D_800E2294;

int func_800CC440(void *arg0, void *arg1, u8 *anim) {
    u16 z;

    *(u16 *)(anim + 6) = D_800E2290;
    *(u16 *)(anim + 8) = D_800E2292;
    z = D_800E2294;
    *(u16 *)(anim + 4) = 0x224;
    anim[3] = 0x7F;
    *(u16 *)(anim + 0xA) = z;
}

#include "common.h"
void func_800C2EAC(int arg0);
void func_800C3098(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3238(int arg0);
void func_800C3B04();

extern u16 D_800F3430;
extern u16 D_800F3432;
extern u16 D_800F3434;
extern int D_800F3440;
extern int D_800F3444;
extern int D_800F3448;
extern short D_800F3458;

int func_800CC480(void *arg0, u8 *params, u8 *anim) {
    short *base_a0;
    int init_a0 = 3;
    register u8 *anim_s1 asm("$17") = anim;
    int value_v0;
    register int value_v1 asm("$3");

    func_800C2EAC(init_a0);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    value_v1 = *(u16 *)(params + 0x2);
    value_v0 = 0x80;
    value_v1 <<= 1;
    value_v0 -= value_v1;
    value_v1 = value_v0;
        value_v0 <<= 16;
    if (value_v0 < 0) {
        value_v1 = 0;
    }

    base_a0 = &D_800F3458;
    base_a0[0] = value_v1;
    asm volatile("" : "=r"(base_a0) : "0"(base_a0));
    base_a0[-20] = *(u16 *)(anim_s1 + 0x6);
    D_800F3432 = *(u16 *)(anim_s1 + 0x8);
    D_800F3434 = *(u16 *)(anim_s1 + 0xA);
    D_800F3440 = *(short *)(anim_s1 + 0x4);
    D_800F3444 = *(short *)(anim_s1 + 0x4) * 2;
    D_800F3448 = *(short *)(anim_s1 + 0x4);
    func_800C3B04(base_a0 - 20);
}

#include "common.h"
#include "pe1/field_anim.h"

/* CC1_FLAGS: -fno-strength-reduce */

void func_800C2EAC(int arg0);
void func_800C3098(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3238(int arg0);
void func_800C3B04();

#define FIELD_ANIM_EXTENT_OFFSET 6

void FieldAnim_ProcessInterleavedPoints(void *arg0, void *arg1, s8 *anim) {
    volatile int stack_pad;
    FieldAnimPointTriple *out;
    u8 *entry;
    unsigned int i;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    D_800E2260.extent_x = *(short *)(anim + FIELD_ANIM_EXTENT_OFFSET);
    D_800E2260.extent_y = *(short *)(anim + FIELD_ANIM_EXTENT_OFFSET);
    D_800E2260.extent_z = *(short *)(anim + FIELD_ANIM_EXTENT_OFFSET);
    D_800E2260.scale = (u8)anim[1];

    if (anim[2] != 0) {
        i = 0;
        out = &D_800E2260.point;
        entry = (u8 *)anim;
        /* This one-shot block preserves retail GCC's s1/s2 allocation. */
        do {
            do {
                FieldAnimInterleavedWindow *window;

                window = (FieldAnimInterleavedWindow *)entry;
                out->x = window->point.x;
                out->y = window->point.y;
                out->z = window->point.z;
                func_800C3B04(out);
                i++;
                entry += 8;
            } while (i < anim[2]);
        } while (0);
    }
}

#include "common.h"
void func_800C2EAC(int arg0);
void func_800C3098(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3238(int arg0);
void func_800C3324(void);

extern u16 D_800F32E0;
extern u16 D_800F32E2;
extern u16 D_800F32E4;
extern int D_800F32F0;
extern int D_800F32F4;
extern int D_800F32F8;
extern short D_800F3308;

int func_800CC644(void *arg0, void *arg1, u8 *anim) {
    u16 value_v0;
    register u16 *output asm("$4");
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(3);

    value_v0 = *(u16 *)(anim + 0x6);
    asm("" : : "r"(value_v0) : "$4");
    output = &D_800F32E0;
    asm("" : "=r"(output) : "0"(output));
    *output = value_v0;
    D_800F32E2 = *(u16 *)(anim + 0x8);
    D_800F32E4 = *(u16 *)(anim + 0xA);
    D_800F32F0 = *(short *)(anim + 0x4);
    D_800F32F4 = *(short *)(anim + 0x4);
    D_800F32F8 = *(short *)(anim + 0x4);
    D_800F3308 = (signed char)anim[3];
    func_800C3324();
}

#include "common.h"
void func_800C2EAC(int arg0);
void func_800C3098(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3238(int arg0);
void func_800C3B04();

extern u16 D_800F3380;
extern u16 D_800F3382;
extern u16 D_800F3384;
extern int D_800F3390;
extern int D_800F3394;
extern int D_800F3398;
extern short D_800F33A8;

int func_800CC6F8(void *arg0, void *arg1, u8 *anim) {
    u16 value_v0;
    register u16 *output asm("$4");
    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    value_v0 = *(u16 *)(anim + 0x6);
    asm("" : : "r"(value_v0) : "$4");
    output = &D_800F3380;
    asm("" : "=r"(output) : "0"(output));
    *output = value_v0;
    D_800F3382 = *(u16 *)(anim + 0x8);
    D_800F3384 = *(u16 *)(anim + 0xA);
    D_800F3390 = *(short *)(anim + 0x4);
    D_800F3394 = *(short *)(anim + 0x4);
    D_800F3398 = *(short *)(anim + 0x4);
    D_800F33A8 = (signed char)anim[3];
    func_800C3B04();
}
