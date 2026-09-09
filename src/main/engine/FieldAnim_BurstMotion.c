#include "common.h"
void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C42A4(void *arg0, void *arg1, int arg2);

extern u8 D_800E27E0;
extern u16 D_800E27EA;
extern int D_800F3478;
extern int D_800F348C;
extern int D_800F3490;
extern int D_800F3494;

int func_800CD50C(void *arg0, void *arg1, u8 *anim) {
    int *base_a1;
    u16 *field_v1;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(3);

    base_a1 = &D_800F348C;
    field_v1 = &D_800E27EA;
    base_a1[0] = *(s16 *)(anim + 0x8);
    D_800F3490 = *(s16 *)(anim + 0xA);
    D_800F3494 = *(s16 *)(anim + 0xC);
    *field_v1 = *(u16 *)(anim + 0x4);
    func_800C42A4((u8 *)field_v1 - 10, base_a1 - 5, 1);
}


void func_800CD59C(void) {
}


#include "common.h"

int func_800CD5A4(void *arg0, u8 *state) {
    int ret = 2;
    state[1] = ret;
    return ret;
}


#include "common.h"
int func_800CD5B0(void *arg0, u8 *state, u8 *anim) {
    *(u16 *)(anim + 4) = *(u16 *)(anim + 4) - 0xA;
    *(u16 *)(anim + 6) = *(u16 *)(anim + 6) + 0x3C;
    if (*(s16 *)(anim + 4) < 0x14) {
        *(u16 *)(anim + 4) = 0;
        state[1] = 2;
    }
}


#include "common.h"
int func_800CD5EC(void *arg0, u8 *state, u8 *anim) {
    unsigned int i = 0;

    do {
        u8 *entry = anim + ((i & 0xFFFF) * 8);

        i++;
        *(u16 *)(entry + 0x10) = *(u16 *)(entry + 0x10) + *(u16 *)(entry + 0x20);
        *(u16 *)(entry + 0x12) = *(u16 *)(entry + 0x12) + *(u16 *)(entry + 0x22);
        *(u16 *)(entry + 0x14) = *(u16 *)(entry + 0x14) + *(u16 *)(entry + 0x24);
    } while ((i & 0xFFFF) < 2);

    anim[3]++;
    if ((signed char)anim[3] >= 8) {
        state[1] = 2;
    }
}


#include "common.h"
int rand(void);

int func_800CD678(void *arg0, u8 *state, u8 *anim) {
    register int value asm("$2") = rand() % 11;
    u16 z = *(u16 *)(anim + 0xA);
    register u16 x asm("$5") = *(u16 *)(anim + 0x4);
    u16 y = *(u16 *)(anim + 0x8);
    z -= 8;
    x -= 8;
    y -= 5;
    asm("" : "=r"(y) : "0"(y));

    *(u16 *)(anim + 0xA) = z;
    *(u16 *)(anim + 0x4) = x;
    y += value;
    *(u16 *)(anim + 0x8) = y;

    if ((short)x < 8) {
        state[1] = 2;
    }
}


#include "common.h"

int func_800CD71C(void *arg0, u8 *state) {
    int ret = 2;
    state[1] = ret;
    return ret;
}
