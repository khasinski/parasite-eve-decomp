#include "common.h"

typedef struct {
    s16 m[3][3];
    s16 pad;
    s32 t[3];
} Matrix;

typedef struct {
    s32 x;
    s32 y;
    s32 z;
    s32 w;
} Vec4;

typedef struct {
    u8 pad00[8];
    s16 rotation[3];
    s16 pad0E;
    s16 firstScale;
    s16 secondScale;
} EffectGeometry;

extern char D_801995F8[];
extern s32 D_8019956C, D_8019957C;

void func_800C3238(int);
void func_80079754(void *, Matrix *);
void func_80071A44(void *, int, int);
void func_80078CC4(Matrix *, Vec4 *);
void func_800C4FC4(void *, Matrix *, int);

void func_80192544(void *unused, char *time, EffectGeometry *record) {
    Matrix matrix;
    Vec4 scratchScale, firstScale, secondScale;

    func_800C3238(3);
    record->rotation[0] = 0;
    record->rotation[1] = 0;
    record->rotation[2] = 0;
    func_80079754(record->rotation, &matrix);
    func_80071A44(&firstScale, 0, 0x10);
    firstScale.x = record->firstScale;
    firstScale.y = record->firstScale;
    firstScale.z = record->firstScale;
    scratchScale = firstScale;
    func_80078CC4(&matrix, &scratchScale);
    matrix.t[0] = D_8019956C;
    matrix.t[1] = 0;
    matrix.t[2] = D_8019957C;
    func_800C4FC4(D_801995F8, &matrix, 1);
    if (*(s16 *)(time + 2) >= 0x3D) {
        func_800C3238(2);
        record->rotation[0] = 0x400;
        record->rotation[1] = 0x400;
        record->rotation[2] = 0;
        func_80079754(record->rotation, &matrix);
        func_80071A44(&secondScale, 0, 0x10);
        secondScale.x = record->secondScale;
        secondScale.y = record->secondScale;
        secondScale.z = record->secondScale;
        firstScale = secondScale;
        func_80078CC4(&matrix, &firstScale);
        matrix.t[0] = D_8019956C;
        matrix.t[1] = 0;
        matrix.t[2] = D_8019957C;
        func_800C4FC4(D_801995F8 + 0x18, &matrix, 0);
    }
}
