#include "common.h"
void **FieldEng_GetSlot(void);

extern int D_800E0F6C;

extern int D_800F33F8;
extern int D_800F33FC;
extern int D_800F3400;
extern u8 D_800F3408;
extern u8 D_800F3409;
extern u8 D_800F340A;
extern u8 D_800F340C;
extern u8 D_800F340D;
extern short D_800F340E;
extern short D_800F33F0;
extern short D_800F33F2;
extern short D_800F33F4;

extern int D_800E2780;
extern int D_800E2784;
extern int D_800E2788;
extern u8 D_800E2790;
extern u8 D_800E2791;
extern u8 D_800E2792;
extern u8 D_800E2794;
extern u8 D_800E2795;
extern short D_800E2796;
extern short D_800E2778;
extern short D_800E277A;
extern short D_800E277C;

extern u8 D_800E27D0;
extern u8 D_800E27D1;
extern u8 D_800E27D2;
extern u8 D_800E27D4;
extern u8 D_800E27D5;
extern short D_800E27D6;
extern short D_800E27B8;
extern short D_800E27BA;
extern short D_800E27BC;

int func_800CD728(void) {
    void **slot = FieldEng_GetSlot();
    int value;
    register int byte2 asm("$4");

    value = (int)&D_800E0F6C;
    *slot = (void *)value;

    value = 0x5F4;
    D_800F33F8 = value;
    D_800F33FC = value;
    D_800F3400 = value;

    value = 0x40;
    byte2 = 0x20;
    asm volatile("" : "=r"(byte2) : "0"(byte2));
    D_800F340C = value;
    value = -0x64;
    D_800F340E = value;

    value = 0x15;
    D_800F3408 = value;

    value = 0x46;
    D_800E27D4 = value;

    value = 0x30;
    D_800E27D5 = value;

    value = -0x6E;
    D_800E27D6 = value;

    value = 0xFF;
    D_800E27D0 = value;

    value = 0xB0;
    D_800E27D1 = value;
    D_800E27D2 = value;

    value = 0xA4;
    D_800E2780 = value;
    D_800E2784 = value;

    value = 0x108;
    D_800E2788 = value;

    value = 0x6E;
    D_800E2794 = value;

    value = 3;
    D_800E2795 = value;

    value = -0x96;
    D_800E2796 = value;

    value = 0x80;
    D_800F33F0 = 0;
    D_800F33F2 = 0;
    D_800F33F4 = 0;
    D_800F340D = byte2;
    D_800F3409 = byte2;
    D_800F340A = byte2;
    D_800E27B8 = 0;
    D_800E27BA = 0;
    D_800E27BC = 0;
    D_800E2778 = 0;
    D_800E277A = 0;
    D_800E277C = 0;
    D_800E2790 = value;
    D_800E2791 = value;
    D_800E2792 = value;

    return 0;
}


extern void FieldEng_Spawn6(int a, int b, int c, int d, int e, int f);

int func_800CD89C(int a, int b, int c, int d, int e, int f) {
    FieldEng_Spawn6(a, b, c, d, e, f);
    return 0;
}


extern char D_800E0F28[];
extern void FieldEng_Register(void *o, void *table);

int func_800CD8C8(void *o) {
    FieldEng_Register(o, D_800E0F28);
    return 0;
}


extern char D_800E0F38[];
extern char D_800E0F18[];
extern char D_800E0F48[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800CD960(u8 *state);

int func_800CD8F0(void *obj) {
    int first;
    int second;
    int status;

    first = func_800C251C(obj, D_800E0F38);
    second = func_800C2758(obj, D_800E0F18, D_800E0F48);
    status = first | second;
    if (status == -1) {
        func_800CD960(obj);
    }
    return 0;
}


#include "common.h"
int func_800CD960(u8 *state) {
    state[0] = 4;
    return 0;
}


int func_800CD970(void) {
    return 0;
}


int func_800CD978(void) {
    return 0;
}


#include "common.h"
int *func_800C2B10(int index);
int *func_800C2B28(int index);
void Akao_Cmd_24(int arg0, int arg1, int arg2, int arg3);
void func_800CEDA8(int arg0);

extern int D_800E2804;
extern u16 D_800E27F0;
extern u16 D_800E27F2;
extern u16 D_800E27F4;
extern u8 D_800B0CE8;
extern int D_800B0E14;

int func_800CD980(void) {
    int index;
    int *table;
    int *entry;
    int *next;
    u8 *data;

    index = *func_800C2B10(0xD);
    table = (int *)*func_800C2B28(1);
    entry = (int *)table[index];
    index++;
    next = (int *)table[index];
    D_800E2804 = (int)entry;

    if (next == 0) {
        *func_800C2B10(0xE) = 1;
    }

    *func_800C2B10(0xD) = index;

    data = (u8 *)D_800E2804;
    D_800E27F0 = *(u16 *)(data + 0x268);
    D_800E27F2 = *(u16 *)(data + 0x26A);
    D_800E27F4 = *(u16 *)(data + 0x26C);

    if (D_800B0CE8 != 0) {
        Akao_Cmd_24(D_800B0E14, 0, 0x80, 0x7F);
    }

    func_800CEDA8(0);
}


#include "common.h"
int rand(void);

extern u16 D_800E27F0;
extern u16 D_800E27F2;
extern u16 D_800E27F4;

int func_800CDA5C(void *arg0, void *arg1, u8 *anim) {
    u16 *base = &D_800E27F0;
    u16 *base_s4;
    int i = 0;
    u8 *entry;
    int value;

    *(u16 *)(anim + 0x4) = base[0];
    *(u16 *)(anim + 0x6) = D_800E27F2;
    *(u16 *)(anim + 0x8) = D_800E27F4;
    *(u16 *)(anim + 0xA) = (rand() % 201) - 0x64;
    *(u16 *)(anim + 0xC) = (rand() % 101) - 0x32;

    base_s4 = base;
    anim[1] = 0x7F;
    *(u16 *)(anim + 0xE) = 0;
    anim[3] = 0;

    do {
        entry = (u8 *)(((i & 0xFFFF) * 2) + (int)anim);
        *(u16 *)(entry + 0x10) = base_s4[0];
        *(u16 *)(entry + 0x20) = base_s4[1];
        *(u16 *)(entry + 0x30) = base_s4[2];
        *(u16 *)(entry + 0x40) = (rand() % 50) - 0x19;
        value = (rand() % 50) - 0x19;
        i++;
        *(u16 *)(entry + 0x60) = 0;
        *(u16 *)(entry + 0x50) = value;
    } while ((unsigned int)(i & 0xFFFF) < 8U);
}


#include "common.h"
int rand(void);

extern u16 D_800E27F0;
extern u16 D_800E27F2;
extern u16 D_800E27F4;

int func_800CDC24(void *arg0, void *arg1, u8 *anim) {
    int value;

    *(u16 *)(anim + 0x4) = D_800E27F0;
    *(u16 *)(anim + 0x6) = D_800E27F2;
    *(u16 *)(anim + 0x8) = D_800E27F4;
    anim[1] = 0x7F;
    *(u16 *)(anim + 0xA) = (rand() % 201) - 0x64;
    value = rand();
    *(u16 *)(anim + 0xE) = 0;
    *(u16 *)(anim + 0xC) = (value % 101) - 0x32;
}


void func_800CDD04(void) {
}
