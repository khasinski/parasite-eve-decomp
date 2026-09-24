#include "common.h"

typedef struct Matrix {
    s16 m[3][3];
    s16 pad;
    s32 t[3];
} Matrix;

typedef struct Vec4 {
    s32 x, y, z, w;
} Vec4;

extern s16 D_801994C2;

void *func_800C2B50(void);
void func_80071A44(Vec4 *, int, int);
void func_80078CC4(Matrix *, Vec4 *);
void func_800C2EAC(u8);
void func_800C2FF0(int, int);
void func_800C3098(int);
void func_800C3238(int);
void func_800C42A4(void *, Matrix *, int);

void func_80190460(void *unused0, void *unused1, char *effect) {
    Matrix matrix;
    Vec4 scratch, draw;
    char *owner;
    char *base = effect;
    register char *position asm("$18");
    register char *scale asm("$16");
    register unsigned i asm("$19");
    int diag = 0x1000;
    s16 *slot = &D_801994C2;
    owner = func_800C2B50();
    i = 0;
        asm volatile("" : : "r"(slot));
    scale = base;
    asm volatile("" : : "r"(scale));
    func_800C2EAC(*(u8 *)(owner + 0x24));
    asm volatile("" : "=r"(position) : "0"(scale));
    func_800C3098(0x10);
    func_800C3238(2);

    for (; i < 4; position += 8, i++, scale += 2) {
        if (*(u8 *)(base + i + 0x3C) == 1) {
            matrix.m[2][2] = diag;
            matrix.m[1][1] = diag;
            matrix.m[0][0] = diag;
            matrix.t[2] = 0;
            matrix.t[1] = 0;
            matrix.t[0] = 0;
            matrix.m[2][1] = 0;
            matrix.m[2][0] = 0;
            matrix.m[1][2] = 0;
            matrix.m[1][0] = 0;
            matrix.m[0][2] = 0;
            matrix.m[0][1] = 0;

            func_80071A44(&draw, 0, 0x10);
            draw.x = *(s16 *)(scale + 0x20);
            draw.y = *(s16 *)(scale + 0x20);
            draw.z = *(s16 *)(scale + 0x20);
            scratch = draw;
            func_80078CC4(&matrix, &scratch);

            matrix.t[0] = *(s16 *)(position + 0);
            matrix.t[1] = *(s16 *)(position + 2);
            matrix.t[2] = *(s16 *)(position + 4);
            *slot = *(u16 *)(scale + 0x28);
            func_800C2FF0(0x10, 0x10);
            *(u8 *)((char *)slot - 6) = *(u8 *)(base + i + 0x30) + (*(u8 *)(base + i + 0x38) >> 1);
            *(u8 *)((char *)slot - 5) = *(u8 *)(base + i + 0x34);
            func_800C42A4((char *)slot - 10, &matrix, 1);
        }
    }
}
