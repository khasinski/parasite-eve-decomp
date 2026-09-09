#include "common.h"
extern void FieldEng_Spawn6(int a, int b, int c, int d, int e, int f);

int func_800C7D00(int a, int b, int c, int d, int e, int f) {
    FieldEng_Spawn6(a, b, c, d, e, f);
    return 0;
}
extern char D_800E0824[];
extern void FieldEng_Register(void *o, void *table);

int func_800C7D2C(void *o) {
    FieldEng_Register(o, D_800E0824);
    return 0;
}
extern char D_800E0840[];
extern char D_800E0808[];
extern char D_800E085C[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800C7DC4(char *obj);

int func_800C7D54(void *obj) {
    int first;
    int second;
    int status;

    first = func_800C251C(obj, D_800E0840);
    second = func_800C2758(obj, D_800E0808, D_800E085C);
    status = first | second;
    if (status == -1) {
        func_800C7DC4(obj);
    }
    return 0;
}
int func_800C7DC4(char *arg0) {
    *arg0 = 4;
    return 0;
}
int func_800C7DD4(void) {
    return 0;
}
int func_800C7DDC(void) {
    return 0;
}
void func_800CEDA8(int arg0);

extern u16 D_800E2348;
extern u16 D_800E234A;
extern u16 D_800E234C;
extern char *D_800E279C;

int func_800C7DE4(char *obj) {
    char *data = *(char **)(obj + 8);

    D_800E2348 = *(int *)(*(char **)(data + 0x238) + 0x274);
    D_800E234A = *(int *)(*(char **)(data + 0x238) + 0x278);
    D_800E234C = *(int *)(*(char **)(data + 0x238) + 0x27C);
    D_800E279C = data;
    func_800CEDA8(0);
}
typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} SVECTOR;

void ApplyMatrixSV(void *matrix, SVECTOR *in, void *out);
int rand(void);

extern char *D_800E279C;
extern u16 D_800E2348;
extern u16 D_800E234A;
extern u16 D_800E234C;

int func_800C7E50(void *arg0, void *arg1, u8 *anim) {
    SVECTOR vec;
    int (*model)[];

    vec.vx = -(rand() % 3 + 9);
    vec.vy = -(rand() % 3 + 9);
    vec.vz = rand() % 5 - 2;

    model = *(int (**)[])(D_800E279C + 0x238);
    ApplyMatrixSV(model, &vec, anim + 0x10);

    *(u16 *)(anim + 0x8) = D_800E2348;
    *(u16 *)(anim + 0xA) = D_800E234A;
    *(u16 *)(anim + 0xC) = D_800E234C;
    anim[2] = 0x14;
    anim[1] = 0;
}
