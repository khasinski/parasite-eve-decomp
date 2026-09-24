#include "common.h"

typedef struct SceneE08Matrix {
    s16 m[3][3];
    s16 pad;
    s32 t[3];
} SceneE08Matrix;

typedef struct SceneE08Vector {
    s32 x, y, z, w;
} SceneE08Vector;

extern char D_801996A0[];
extern char D_80198848[];
extern s32 D_8019956C, D_8019957C;

void *func_800C2B50(void);
void func_800C2EAC(u8);
void func_800C2FF0(int, int);
void func_800C3098(int);
void func_800C3238(int);
int func_80077CF4(int);
int func_80077DC4(int);
void func_80071A44(SceneE08Vector *, int, int);
void func_80078CC4(SceneE08Matrix *,SceneE08Vector *);
void func_800C3134(void *, int, void *);
void func_800C42A4(void *, SceneE08Matrix *, int);
void func_80192D90(void *unused0, void *unused1, char *effect) {
    SceneE08Matrix matrix;
    s16 temp[3];
    SceneE08Vector transformed, v;
    /* Keep the matrix scale live before the first rendering setup call. */
    register s16 scale asm("$19") = 0x1000;
    register char *table asm("$20") = D_801996A0;
    char *owner;
    unsigned i;
    /* Both cursors address interleaved effect fields at different strides. */
    register char *phase asm("$16") = effect;
    char *state;

    owner = func_800C2B50();
    i = 0;
    asm volatile("" : : "r"(scale));
    state = phase;
    func_800C2EAC(*(u8 *)(owner + 0x24));
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
    for (; i < 32; i++) {
        if (*(u8 *)state == 1) {
            temp[0] = (func_80077CF4(*(s16 *)(phase + 0x40)) * *(s16 *)(phase + 0xC0)) >> 12;
            temp[1] = 0;
            temp[2] = (func_80077DC4(*(s16 *)(phase + 0x40)) * *(s16 *)(phase + 0xC0)) >> 12;
            matrix.m[2][2] = scale;
            matrix.m[1][1] = scale;
            matrix.m[0][0] = scale;
            matrix.t[2] = 0;
            matrix.t[1] = 0;
            matrix.t[0] = 0;
            matrix.m[2][1] = 0;
            matrix.m[2][0] = 0;
            matrix.m[1][2] = 0;
            matrix.m[1][0] = 0;
            matrix.m[0][2] = 0;
            matrix.m[0][1] = 0;
            func_80071A44(&v, 0, 0x10);
            v.x = *(s16 *)(phase + 0x80);
            v.y = *(s16 *)(phase + 0x80);
            v.z = scale;
            transformed = v;
            func_80078CC4(&matrix, &transformed);
            matrix.t[0] = temp[0] + D_8019956C;
            matrix.t[1] = temp[1];
            matrix.t[2] = temp[2] + D_8019957C;
            func_800C3134(D_80198848, *(u8 *)(state + 0x20), table);
            func_800C42A4(table, &matrix, 1);
        }
        phase += 2;
        state += 1;
    }
}
