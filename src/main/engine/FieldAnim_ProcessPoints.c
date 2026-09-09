#include "common.h"
#include "pe1/field_anim.h"
void func_800C2EAC(int arg0);
void func_800C3098(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3238(int arg0);
void func_800C3B04(void *arg0);

void FieldAnim_ProcessPointTriples(void *arg0, void *arg1,
                                   FieldAnimPointData *points) {
    volatile int stack_pad;
    FieldAnimPointTriple *out;
    unsigned int i;

    i = 0;
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x10, 0x10);
    func_800C3238(0);

    D_800E2818.scale = (s32)(points->scale << 24) >> 23;

    if (points->count != 0) {
        out = &D_800E2818.point;
        /* This one-shot block preserves retail GCC's s1/s2 allocation. */
        do {
            do {
                out->x = points->x[i];
                out->y = points->y[i];
                out->z = points->z[i];
                func_800C3B04(out);
                i++;
            } while (i < points->count);
        } while (0);
    }
}

#include "common.h"
void func_800C2EAC(int arg0);
void func_800C3098(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3238(int arg0);
void func_800C3324(void);

extern u16 D_800F3338;
extern u16 D_800F333A;
extern u16 D_800F333C;
extern int D_800F3348;
extern int D_800F334C;
extern int D_800F3350;
extern short D_800F3360;

int func_800CC878(void *arg0, void *arg1, u8 *anim) {
    u16 value_v0;
    register u16 *output asm("$4");
    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    value_v0 = *(u16 *)(anim + 0x6);
    asm("" : : "r"(value_v0) : "$4");
    output = &D_800F3338;
    asm("" : "=r"(output) : "0"(output));
    *output = value_v0;
    D_800F333A = *(u16 *)(anim + 0x8);
    D_800F333C = *(u16 *)(anim + 0xA);
    D_800F3348 = *(short *)(anim + 0x4);
    D_800F334C = *(short *)(anim + 0x4);
    D_800F3350 = *(short *)(anim + 0x4);
    D_800F3360 = (signed char)anim[3];
    func_800C3324();
}

#include "common.h"
int func_800CC92C(void *arg0, u8 *state, u8 *anim) {
    u16 value;
    register int frame asm("$3");

    value = *(u16 *)(anim + 6);
    frame = anim[3];
    *(u16 *)(anim + 6) = value;
    value = *(u16 *)(anim + 8);
    frame -= 2;
    anim[3] = frame;
    asm("" ::: "memory");
    frame = *(signed char *)(anim + 3);
    value -= 0xA;
    *(u16 *)(anim + 8) = value;
    value = *(u16 *)(anim + 4);
    frame = frame < 0x1E;
    value += 0x1E;
    *(u16 *)(anim + 4) = value;
    if (frame) {
        state[1] = 2;
    }
}

#include "common.h"
int func_800CC974(void *arg0, u8 *state, u8 *anim) {
    int i;
    u8 *state_t2;
    register u8 *timer_t0 asm("$8");
    register u8 *entry_a3 asm("$7");
    register int dx asm("$3");
    int dy;
    register int dz asm("$5");
    int frame;

        state_t2 = state;
    asm volatile("" : "=r"(state_t2) : "0"(state_t2));
    i = 0;
    if ((signed char)anim[2] > 0) {
        timer_t0 = anim;
        asm volatile("" : "=r"(timer_t0) : "0"(timer_t0));
        entry_a3 = anim;
        do {
            register int pos asm("$2");
            register int angle asm("$3");

            i++;
            dx = *(short *)(entry_a3 + 0xA6) >> 9;
            pos = *(u16 *)(entry_a3 + 0x26);
            dy = *(short *)(entry_a3 + 0xA8) >> 9;
            dz = *(short *)(entry_a3 + 0xAA) >> 9;
            pos += dx;
            *(u16 *)(entry_a3 + 0x26) = pos;
            pos = *(u16 *)(entry_a3 + 0x28);
            angle = *(u16 *)(entry_a3 + 0xA8);
            pos += dy;
            *(u16 *)(entry_a3 + 0x28) = pos;
            pos = *(u16 *)(entry_a3 + 0x2A);
            angle += 0xB4;
            *(u16 *)(entry_a3 + 0xA8) = angle;
            pos += dz;
            *(u16 *)(entry_a3 + 0x2A) = pos;
            pos = *(u16 *)(timer_t0 + 0x6);
            entry_a3 += 8;
            pos += 0x18;
            *(u16 *)(timer_t0 + 0x6) = pos;
            timer_t0 += 2;
        } while (i < (signed char)anim[2]);
    }

    anim[1] -= 2;
    if (anim[1] < 2) {
        state_t2[1] = 2;
    }
    asm volatile("" : : "r"(&frame));
}

#include "common.h"
int func_800CCA40(void *arg0, u8 *state, u8 *anim) {
    anim[3] -= 2;
    *(u16 *)(anim + 4) = *(u16 *)(anim + 4) + 0x28;
    if (*(s8 *)(anim + 3) < 0x1E) {
        state[1] = 2;
    }
}

#include "common.h"
int func_800CCA78(void *arg0, u8 *state, u8 *anim) {
    anim[3] -= 6;
    *(u16 *)(anim + 4) = *(u16 *)(anim + 4) + 0xB4;
    if (*(s8 *)(anim + 3) < 0x1E) {
        state[1] = 2;
    }
}

#include "common.h"
int func_800CCAB0(void *arg0, u8 *state, u8 *anim) {
    int i;
    u8 *state_t1;
    register u8 *entry_a3 asm("$7");
    register int dx asm("$3");
    int dy;
    register int dz asm("$5");
    int frame;

        state_t1 = state;
    asm volatile("" : "=r"(state_t1) : "0"(state_t1));
    i = 0;
    if (*(short *)(anim + 0x4) > 0) {
        entry_a3 = anim;
        do {
            register int pos asm("$2");
            register int angle asm("$3");

            i++;
            dx = *(short *)(entry_a3 + 0x68) >> 8;
            pos = *(u16 *)(entry_a3 + 0x8);
            dy = *(short *)(entry_a3 + 0x88) >> 8;
            dz = *(short *)(entry_a3 + 0xA8) >> 8;
            pos += dx;
            *(u16 *)(entry_a3 + 0x8) = pos;
            pos = *(u16 *)(entry_a3 + 0x28);
            angle = *(u16 *)(entry_a3 + 0x88);
            pos += dy;
            *(u16 *)(entry_a3 + 0x28) = pos;
            pos = *(u16 *)(entry_a3 + 0x48);
            angle += 0xB4;
            *(u16 *)(entry_a3 + 0x88) = angle;
            pos += dz;
            *(u16 *)(entry_a3 + 0x48) = pos;
            entry_a3 += 2;
        } while (i < *(short *)(anim + 0x4));
    }

    anim[3] -= 2;
    if ((signed char)anim[3] < 2) {
        state_t1[1] = 2;
    }
    asm volatile("" : : "r"(&frame));
}

#include "common.h"
int func_800CCB6C(void *arg0, u8 *state, u8 *anim) {
    anim[3] -= 8;
    *(u16 *)(anim + 4) = *(u16 *)(anim + 4) + 0x1A4;
    if (*(s8 *)(anim + 3) < 0x14) {
        anim[3] = 0;
        state[1] = 2;
    }
}
