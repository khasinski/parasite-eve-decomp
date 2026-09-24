#include "common.h"

typedef struct {
    s16 m[3][3];
    s16 pad;
    s32 t[3];
} Matrix;

typedef struct {
    s32 x, y, z, w;
} Vec4;

extern s16 D_8019955A;
extern char D_80199580[];
extern s16 D_800942EC;

void *func_800C2B50(void);
void func_800C2EAC(u8);
void func_800C2FF0(int, int);
void func_800C3098(int);
void func_800C3238(int);
void func_800794C4(void *, Matrix *);
void func_80071A44(Vec4 *, int, int);
void func_80078CC4(Matrix *, Vec4 *);
void func_800C42A4(void *, Matrix *, int);
void func_80194090(void *arg0, void *arg1, char *effects) {
    Matrix matrix;
    Vec4 scratch, vec;
    char *owner;
    s32 third;
    register char *base asm("$20") = effects;
    register char *state asm("$17");
    register char *phase asm("$16");
    register s16 *alpha asm("$21");
    register unsigned i asm("$18");
    register unsigned angleOffset asm("$19");
    owner = func_800C2B50();
    i = 0;
    alpha = &D_8019955A;
    /* This single-pass block preserves the original setup scheduling. */
    do {
        state = base;
        phase = base;
        angleOffset = 0x30;
        func_800C2EAC(*(u8 *)(owner + 0x24));
        func_800C2FF0(0x40, 0x20);
        func_800C3098(0x10);
        func_800C3238(2);
    } while (0);

    for (; i < 3; ++i, angleOffset += 8) {
        func_800794C4(base + angleOffset, &matrix);
        func_80071A44(&vec, 0, 0x10);
        vec.x = *(s16 *)(phase + 0x48);
        vec.y = *(s16 *)(phase + 0x4A);
        vec.z = *(s16 *)(phase + 0x4C);
        scratch = vec;
        func_80078CC4(&matrix, &scratch);
        matrix.t[0] = *(s32 *)(state + 0);
        matrix.t[1] = *(s32 *)(state + 4);
        third = *(s32 *)(state + 8);
        *alpha = 0x80 - (i << 5);
        matrix.t[2] = third;
        if (i == 0) {
            *alpha = 0xC0;
        }
        func_800C42A4((char *)alpha - 10, &matrix, 0);
        state += 0x10;
        phase += 8;
    }
    matrix.t[0] = *(s32 *)(base + 0);
    matrix.t[1] = D_800942EC;
    matrix.t[2] = *(s32 *)(base + 8);
    func_800C42A4(D_80199580, &matrix, 0);
}
