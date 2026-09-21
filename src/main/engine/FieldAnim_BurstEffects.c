#include "pe1/field_anim.h"
#include "pe1/field_engine_state.h"

static __inline__ void identity_matrix(GteMatrix *matrix)
{
    matrix->m[0][0] = matrix->m[1][1] = matrix->m[2][2] = 4096;
    matrix->m[0][1] = matrix->m[0][2] = matrix->m[1][0] =
        matrix->m[1][2] = matrix->m[2][0] = matrix->m[2][1] =
        matrix->t[0] = matrix->t[1] = matrix->t[2] = 0;
}

int func_800CCBA8(char *object)
{
    GteVector first, second, third;

    *FieldEng_GetSlot(object) = D_800E0EB8;
    first = D_800C220C;
    identity_matrix(&D_800F3478);
    Gte_ScaleMatrix(&D_800F3478, &first);
    second = D_800C221C;
    identity_matrix(&D_800F33C0);
    Gte_ScaleMatrix(&D_800F33C0, &second);
    third = D_800C222C;
    identity_matrix(&D_800F32B0);
    Gte_ScaleMatrix(&D_800F32B0, &third);

    D_800E2298.parameter04 = 0x42;
    D_800E2298.parameter05 = 0x20;
    D_800E2298.parameter08 = 50;
    D_800E2298.parameter0A = 127;
    D_800E2298.r = 128;
    D_800E2298.g = 128;
    D_800E2298.b = 128;

    D_800E2250.parameter04 = 128;
    D_800E2250.parameter05 = 4;
    D_800E2250.parameter08 = -50;
    D_800E2250.parameter0A = 127;
    D_800E2250.r = 128;
    D_800E2250.g = 128;
    D_800E2250.b = 128;

    D_800E27E0.parameter04 = 0x68;
    D_800E27E0.parameter05 = 0;
    D_800E27E0.parameter08 = -99;
    D_800E27E0.parameter0A = 127;
    D_800E27E0.r = 64;
    D_800E27E0.b = 64;
    D_800E27E0.g = 128;

    D_800F3460.parameter04 = 0x6E;
    D_800F3460.parameter05 = 3;
    D_800F3460.parameter08 = -100;
    D_800F3460.parameter0A = 127;
    D_800F3460.r = 128;
    D_800F3460.g = 128;
    D_800F3460.b = 128;
    return 0;
}

#include "pe1/field_anim.h"

extern u16 D_800E27FA __asm__("D_800E27FA");
extern u16 D_800E27FC __asm__("D_800E27FC");

extern void FieldEng_Spawn6(int a, int b, int c, int d, int e, int f);

int func_800CCEBC(int a, int b, int c, int d, int e, int f) {
    FieldEng_Spawn6(a, b, c, d, e, f);
    return 0;
}


extern char D_800E0E60[];
extern void FieldEng_Register(void *o, void *table);

int func_800CCEE8(void *o) {
    FieldEng_Register(o, D_800E0E60);
    return 0;
}


extern char D_800E0E78[];
extern char D_800E0E48[];
extern char D_800E0E90[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800CCF80(u8 *state);

int func_800CCF10(void *obj) {
    int first;
    int second;
    int status;

    first = func_800C251C(obj, D_800E0E78);
    second = func_800C2758(obj, D_800E0E48, D_800E0E90);
    status = first | second;
    if (status == -1) {
        func_800CCF80(obj);
    }
    return 0;
}


#include "common.h"
int func_800CCF80(u8 *state) {
    state[0] = 4;
    return 0;
}


int func_800CCF90(void) {
    return 0;
}


int func_800CCF98(void) {
    return 0;
}


#include "common.h"
int *func_800C2B10(int index);
int *func_800C2B28(int index);
void Akao_Cmd_24(int arg0, int arg1, int arg2, int arg3);
void func_800CEDA8(int arg0);

extern int D_800E2810;
extern u8 D_800B0CE8;
extern int D_800B0E14;

int func_800CCFA0(void) {
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
    D_800E2810 = (int)entry;

    if (next == 0) {
        *func_800C2B10(0xE) = 1;
    }

    *func_800C2B10(0xD) = index;

    data = (u8 *)D_800E2810;
    D_800E27F8.x = *(u16 *)(data + 0x268);
    D_800E27FA = *(u16 *)(data + 0x26A);
    D_800E27FC = *(u16 *)(data + 0x26C);

    if (D_800B0CE8 != 0) {
        Akao_Cmd_24(D_800B0E14, 0, 0x80, 0x7F);
    }

    func_800CEDA8(0);
}


#include "common.h"
extern char *D_8009D254;

int func_800CD07C(void *arg0, void *arg1, u8 *anim) {
    u16 x = D_800E27F8.x;
    char *entity = D_8009D254;
    int y;
    u16 z;

    *(u16 *)(anim + 8) = x;
    y = *(s16 *)(entity + 0x2E);
    *(u16 *)(anim + 0xA) = y;
    z = D_800E27FC;
    *(u16 *)(anim + 4) = 0x7F;
    *(u16 *)(anim + 6) = 0x60C;
    *(u16 *)(anim + 0xC) = z;
}


#include "common.h"
#include "pe1/field_anim.h"

int rand(void);

void FieldAnim_InitRandomOffsetPairs(void *arg0, void *arg1,
                                     FieldAnimBurstData *anim) {
    FieldAnimPointTriple *base;
    u16 *base_values;
    FieldAnimBurstWindow *window;
    u8 *entry;
    u16 z;
    int i;

    base = &D_800E27F8;
    i = 0;

    *(u16 *)anim->header.origin_x = base->x;
    *(u16 *)anim->header.origin_y = base->y;
    z = base->z;
    *(u16 *)anim->header.scale = 0x7F;
    *(u16 *)anim->header.duration = 0x3B4;
    anim->header.mode = 0;
    *(u16 *)anim->header.origin_z = z;
    base_values = &base->x;

    do {
        entry = &anim->bytes[(i & 0xFFFF) * 8];
        window = (FieldAnimBurstWindow *)entry;
        window->point.x = base_values[0];
        window->point.y = base_values[1];
        window->point.z = base_values[2];
        window->offset.x = (rand() % 50) - 0x19;
        window->offset.y = (rand() % 50) - 0x19;
        i++;
        window->offset.z = 0;
    } while (((unsigned int)i & 0xFFFF) < 2);
}


#include "common.h"
int rand(void);


int func_800CD1FC(void *arg0, void *arg1, u8 *anim) {
    u16 z;

    *(u16 *)(anim + 0x8) = (D_800E27F8.x + (rand() % 201)) - 0x64;
    *(u16 *)(anim + 0xA) = (D_800E27FA + (rand() % 101)) - 0x32;
    z = D_800E27FC;
    *(u16 *)(anim + 0x4) = 0x7F;
    *(u16 *)(anim + 0x6) = 0x544;
    *(u16 *)(anim + 0xC) = z;
}
