#include "common.h"
typedef struct {
    s16 m[3][3];
    int t[3];
} Matrix;

void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C42A4(void *arg0, void *arg1, int arg2);
void Gte_ScaleMatrix(Matrix *matrix, int *scale);
void *memset(void *dest, int value, unsigned int count);

extern u8 D_800E2338;
extern u16 D_800E2342;

int func_800CB750(void *arg0, void *arg1, u8 *anim) {
    u16 *field_s4 = &D_800E2342;
    Matrix matrix;
    int scale;
    u16 field;
    int localScale[4];
    int localScale2[4];

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    scale = *(s16 *)(anim + 0x6) + 0x42C;
    field = *(u16 *)(anim + 0x4);

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;

    *field_s4 = field;
    matrix.t[0] = *(s16 *)(anim + 0x8);
    matrix.t[1] = *(s16 *)(anim + 0xA);
    matrix.t[2] = *(s16 *)(anim + 0xC);

    memset(localScale, 0, sizeof(localScale));
    localScale[0] = scale;
    localScale[1] = scale;
    localScale[2] = scale;

    Gte_ScaleMatrix(&matrix, localScale);
    func_800C42A4((u8 *)field_s4 - 10, &matrix, 1);

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;

    matrix.t[0] = *(s16 *)(anim + 0x10);
    matrix.t[1] = *(s16 *)(anim + 0x12);
    matrix.t[2] = *(s16 *)(anim + 0x14);

    memset(localScale2, 0, sizeof(localScale2));
    localScale2[0] = scale;
    localScale2[1] = scale;
    localScale2[2] = scale;

    Gte_ScaleMatrix(&matrix, localScale2);
    func_800C42A4((u8 *)field_s4 - 10, &matrix, 1);
}

#include "common.h"
typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} SVECTOR;


void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C42A4(void *arg0, void *arg1, int arg2);
void Gte_ScaleMatrix(Matrix *matrix, int *scale);
void RotMatrix(SVECTOR *rot, Matrix *matrix);
void *memset(void *dest, int value, unsigned int count);

extern SVECTOR D_800C2204;
extern u8 D_800F34E8;
extern s16 D_800F34F2;

int func_800CB8E0(void *arg0, void *arg1, u8 *anim) {
    u16 *field_s1 = &D_800F34F2;
    Matrix *matrix_a0;
    Matrix matrix;
    SVECTOR rot;
    int scale_arg[4];
    volatile int scale[4];
    register int scale2_a2 asm("$6");
    register int scale3_a3 asm("$7");

    rot = D_800C2204;

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    *field_s1 = *(unsigned short *)(anim + 0x4);
    RotMatrix(&rot, &matrix);

    matrix.t[0] = *(s16 *)(anim + 0x8);
    matrix.t[1] = *(s16 *)(anim + 0xA);
    matrix.t[2] = *(s16 *)(anim + 0xC);

    memset(scale, 0, sizeof(scale));
    scale[0] = *(s16 *)(anim + 0x6);
    scale[1] = *(s16 *)(anim + 0x6);
    matrix_a0 = &matrix;
    asm volatile("" : "=r"(matrix_a0) : "0"(matrix_a0));
    scale[2] = *(s16 *)(anim + 0x6);

    scale_arg[0] = scale[0];
    scale_arg[1] = scale[1];
    scale2_a2 = scale[2];
    scale3_a3 = scale[3];
    scale_arg[2] = scale2_a2;
    scale_arg[3] = scale3_a3;
    asm volatile("" ::: "$5");

    Gte_ScaleMatrix(matrix_a0, scale_arg);
    func_800C42A4((u8 *)field_s1 - 10, &matrix, 0);
}

#include "common.h"

void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C42A4(void *arg0, void *arg1, int arg2);
void Gte_ScaleMatrix(Matrix *matrix, int *scale);
void *memset(void *dest, int value, unsigned int count);

extern u8 D_800F34E8;
extern s16 D_800F34F2;

int func_800CB9F8(void *arg0, void *arg1, u8 *anim) {
    u16 *field_s2 = &D_800F34F2;
    Matrix *matrix_s1;
    Matrix *call_a0;
    int scale_v0;
    register int scale2_a2 asm("$6");
    register int scale3_a3 asm("$7");
    Matrix matrix;
    int scale_arg[4];
    volatile int scale[4];
    int m0;
    int m1;
    int m2;
    int m3;

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    *field_s2 = *(unsigned short *)(anim + 0x4);

    m0 = *(int *)(anim + 0x10);
    m1 = *(int *)(anim + 0x14);
    m2 = *(int *)(anim + 0x18);
    m3 = *(int *)(anim + 0x1C);
    *(int *)((char *)&matrix + 0x0) = m0;
    *(int *)((char *)&matrix + 0x4) = m1;
    *(int *)((char *)&matrix + 0x8) = m2;
    *(int *)((char *)&matrix + 0xC) = m3;
    m0 = *(int *)(anim + 0x20);
    m1 = *(int *)(anim + 0x24);
    m2 = *(int *)(anim + 0x28);
    m3 = *(int *)(anim + 0x2C);
    *(int *)((char *)&matrix + 0x10) = m0;
    *(int *)((char *)&matrix + 0x14) = m1;
    *(int *)((char *)&matrix + 0x18) = m2;
    *(int *)((char *)&matrix + 0x1C) = m3;

    matrix.t[0] = *(s16 *)(anim + 0x8);
    matrix.t[1] = *(s16 *)(anim + 0xA);
    matrix.t[2] = *(s16 *)(anim + 0xC);

    memset(scale, 0, sizeof(scale));
    m0 = *(s16 *)(anim + 0x6);
    matrix_s1 = &matrix;
    scale[0] = m0;
    scale_v0 = *(s16 *)(anim + 0x6);
    asm("" : : "r"(scale_v0) : "$4");
    call_a0 = matrix_s1;
    asm volatile("" : "=r"(call_a0) : "0"(call_a0));
    scale[1] = scale_v0;
    scale[2] = *(s16 *)(anim + 0x6);

    scale_arg[0] = scale[0];
    scale_arg[1] = scale[1];
    scale2_a2 = scale[2];
    scale3_a3 = scale[3];
    scale_arg[2] = scale2_a2;
    scale_arg[3] = scale3_a3;

    asm volatile("" ::: "memory");
    Gte_ScaleMatrix(call_a0, scale_arg);
    func_800C42A4((u8 *)field_s2 - 10, matrix_s1, 0);
}

#include "common.h"

int func_800CBB24(void *arg0, u8 *state) {
    int ret = 2;

    state[1] = ret;
    return ret;
}

#include "common.h"
int func_800CBB30(void *arg0, u8 *state, u8 *anim) {
    u8 *state_a3 = state;
    u8 *anim_a2 = anim;
    int temp_v0;
    int temp_v1;
    register int temp_a0 asm("$4");
    register int temp_a1 asm("$5");
    u8 count;

    temp_v0 = *(u16 *)(anim_a2 + 0x8);
    temp_v1 = *(u16 *)(anim_a2 + 0x10);
    temp_a0 = *(u16 *)(anim_a2 + 0x12);
    temp_a1 = *(u16 *)(anim_a2 + 0x14);

    temp_v0 += temp_v1;
    *(u16 *)(anim_a2 + 0x8) = temp_v0;
    temp_v0 = *(u16 *)(anim_a2 + 0xA);
    temp_v1 = *(u16 *)(anim_a2 + 0xC);
    temp_v0 += temp_a0;
    temp_v1 += temp_a1;
    *(u16 *)(anim_a2 + 0xA) = temp_v0;
    temp_v0 = *(u16 *)(anim_a2 + 0x12);
    temp_a0 = (unsigned int)anim_a2;
    *(u16 *)(anim_a2 + 0xC) = temp_v1;
    temp_v1 = anim_a2[1];
    temp_v0 += 3;
    *(u16 *)(anim_a2 + 0x12) = temp_v0;
    temp_v0 = *(s16 *)(anim_a2 + 0xA);
    temp_v1++;
    anim_a2[1] = temp_v1;
    if (temp_v0 > 0) {
        *(u16 *)(anim_a2 + 0x12) = -*(u16 *)(anim_a2 + 0x12);
    }
    count = ((u8 *)temp_a0)[2];
    ((u8 *)temp_a0)[2] = count - 1;
    if (count == 0) {
        state_a3[1] = 2;
    }
}

#include "common.h"
int func_800CBBBC(void *arg0, u8 *state, u8 *anim) {
    s16 value = *(u16 *)(anim + 4) - 0x14;

    *(u16 *)(anim + 4) = value;
    if (value < 0x14) {
        *(u16 *)(anim + 4) = 0;
        state[1] = 2;
    }
}

#include "common.h"
int func_800CBBF0(void *arg0, u8 *state, u8 *anim) {
    *(u16 *)(anim + 4) = *(u16 *)(anim + 4) - 8;
    *(u16 *)(anim + 6) = *(u16 *)(anim + 6) + 0x3C;
    if (*(s16 *)(anim + 4) < 0x14) {
        *(u16 *)(anim + 4) = 0;
        state[1] = 2;
    }
}

#include "common.h"
int func_800CBC2C(void *arg0, u8 *state, u8 *anim) {
    *(u16 *)(anim + 4) = *(u16 *)(anim + 4) - 8;
    *(u16 *)(anim + 6) = *(u16 *)(anim + 6) + 0x78;
    if (*(s16 *)(anim + 4) < 0x14) {
        *(u16 *)(anim + 4) = 0;
        state[1] = 2;
    }
}

#include "common.h"
int func_800CBC68(void *arg0, u8 *state, u8 *anim) {
    *(u16 *)(anim + 4) = *(u16 *)(anim + 4) - 8;
    *(u16 *)(anim + 6) = *(u16 *)(anim + 6) + 0xA;
    if (*(s16 *)(anim + 4) < 0x14) {
        *(u16 *)(anim + 4) = 0;
        state[1] = 2;
    }
}

#include "common.h"
#define S16(base, off) (*(s16 *)((u8 *)(base) + (off)))
#define PTR(base, off) (*(u8 **)((u8 *)(base) + (off)))

extern u32 *FieldEng_GetSlot(int slot);

extern u8 D_800E0D6C[];
extern volatile s16 D_800E2290_state __asm__("D_800E2290");
extern volatile s16 D_800E2292_state __asm__("D_800E2292");
extern volatile s16 D_800E2294_state __asm__("D_800E2294");
extern volatile u8 D_800E2284;
extern volatile u8 D_800E2285;
extern volatile s16 D_800E2286;
extern volatile s16 D_800E2268;
extern volatile s16 D_800E226A;
extern volatile s16 D_800E226C;
extern volatile s16 D_800E2820;
extern volatile s16 D_800E2822;
extern volatile s16 D_800E2824;
extern volatile u32 D_800E2828;
extern volatile u32 D_800E282C;
extern volatile u32 D_800E2830;
extern volatile u8 D_800E2838;
extern volatile u8 D_800E2839;
extern volatile u8 D_800E283A;
extern volatile u8 D_800E283C;
extern volatile u8 D_800E283D;
extern volatile s16 D_800E283E;

extern volatile u8 *D_800F3470;
extern volatile u8 D_800F3454;
extern volatile u8 D_800F3455;
extern volatile s16 D_800F3456;
extern volatile s16 D_800F3438;
extern volatile s16 D_800F343A;
extern volatile s16 D_800F343C;
extern volatile s16 D_800F32E8;
extern volatile s16 D_800F32EA;
extern volatile s16 D_800F32EC;
extern volatile u8 D_800F3300;
extern volatile u8 D_800F3301;
extern volatile u8 D_800F3302;
extern volatile u8 D_800F3304;
extern volatile u8 D_800F3305;
extern volatile s16 D_800F3306;
extern volatile s16 D_800F3388;
extern volatile s16 D_800F338A;
extern volatile s16 D_800F338C;
extern volatile u8 D_800F33A4;
extern volatile u8 D_800F33A5;
extern volatile s16 D_800F33A6;
extern volatile u8 D_800F335C;
extern volatile u8 D_800F335D;
extern volatile s16 D_800F335E;
extern volatile u8 D_800F33A0;
extern volatile u8 D_800F33A1;
extern volatile u8 D_800F33A2;
extern volatile s16 D_800F3340;
extern volatile s16 D_800F3342;
extern volatile s16 D_800F3344;
extern volatile u8 D_800F3358;
extern volatile u8 D_800F3359;
extern volatile u8 D_800F335A;

int func_800CBCA4(u8 *arg0) {
    u8 *obj;
    register int tmp asm("$2");
    int y_scale;
    int color;
    register int z asm("$6");
    register int orange asm("$4");
    register int y_off asm("$4");
    u32 *slot;

    slot = FieldEng_GetSlot((int)arg0);
    *slot = (u32)D_800E0D6C;
    obj = PTR(arg0, 8);

    tmp = S16(obj, 0x2A);
    y_off = -0x64;
    D_800E2290_state = tmp;
    tmp = S16(obj, 0x2E);
    y_scale = 0x400;
    D_800F3470 = obj;
    D_800E2292_state = tmp;

    tmp = 0;
    asm volatile("" : "=r"(tmp) : "0"(tmp) : "memory");
    z = S16(obj, 0x32);
    color = 0x68;
    D_800F3454 = color;
    D_800E2284 = color;
    D_800F3304 = color;

    color = 0xA0;
    D_800F3300 = color;
    D_800F3301 = color;
    D_800F3302 = color;

    color = 0x46;
    D_800F3456 = y_off;
    D_800F3306 = y_off;

    orange = 0x80;
    D_800F3438 = 0;
    D_800F343A = 0;
    D_800F343C = 0;
    D_800F3455 = 0;

    D_800E2268 = y_scale;
    D_800E226A = 0;
    D_800E226C = 0;
    D_800E2285 = 0;
    D_800E2286 = 0;

    D_800F32E8 = y_scale;
    D_800F32EA = 0;
    D_800F32EC = 0;
    D_800F3305 = 0;

    D_800F3388 = y_scale;
    D_800F338A = 0;
    D_800F338C = 0;

    D_800E2294_state = z;
    D_800F33A4 = color;

    color = 0x30;
    D_800F33A5 = color;

    color = -0x50;
    D_800F33A6 = color;

    color = 0x1A8;
    D_800E2828 = color;
    D_800E282C = color;
    D_800E2830 = color;

    color = 0xBC;
    D_800E283C = color;

    color = 8;
    D_800E283D = color;

    color = 0x42;
    D_800F335C = color;

    color = 0x20;
    D_800F335D = color;

    color = 0x32;
    D_800F33A0 = orange;
    D_800F33A1 = orange;
    D_800F33A2 = orange;

    D_800E2820 = 0;
    D_800E2822 = 0;
    D_800E2824 = 0;
    D_800E283E = 0;
    D_800E2838 = orange;
    D_800E2839 = orange;
    D_800E283A = orange;

    D_800F3340 = y_scale;
    D_800F3342 = 0;
    D_800F3344 = 0;
    D_800F335E = color;
    D_800F3358 = orange;
    D_800F3359 = orange;
    D_800F335A = orange;
    return tmp;
}

extern void FieldEng_Spawn6(int a, int b, int c, int d, int e, int f);

int func_800CBEE0(int a, int b, int c, int d, int e, int f) {
    FieldEng_Spawn6(a, b, c, d, e, f);
    return 0;
}

extern char D_800E0D08[];
extern void FieldEng_Register(void *o, void *table);

int func_800CBF0C(void *o) {
    FieldEng_Register(o, D_800E0D08);
    return 0;
}

extern char D_800E0D24[];
extern char D_800E0CEC[];
extern char D_800E0D40[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800CBFA4(char *obj);

int func_800CBF34(void *obj) {
    int first;
    int second;
    int status;

    first = func_800C251C(obj, D_800E0D24);
    second = func_800C2758(obj, D_800E0CEC, D_800E0D40);
    status = first | second;
    if (status == -1) {
        func_800CBFA4(obj);
    }
    return 0;
}

int func_800CBFA4(char *arg0) {
    *arg0 = 4;
    return 0;
}

int func_800CBFB4(void) {
    return 0;
}

int func_800CBFBC(void) {
    return 0;
}

#include "common.h"
int rand(void);
int *func_800C2B10(int index);

extern u8 D_800F3450;
extern u8 D_800F3451;
extern u8 D_800F3452;
extern u16 D_800E2290;
extern u16 D_800E2292;
extern u16 D_800E2294;

int func_800CBFC4(void *arg0, void *arg1, u8 *anim) {
    int x_base_a1;
    int z_base_a1;
    int x_rand;
    int z_rand;

    D_800F3450 = *func_800C2B10(0);
    D_800F3451 = *func_800C2B10(1);
    D_800F3452 = *func_800C2B10(2);

    x_rand = rand();
    x_base_a1 = D_800E2290 - 0x64;
    *(u16 *)(anim + 0x6) = x_base_a1 + (x_rand % 201);
    *(u16 *)(anim + 0x8) = D_800E2292;

    z_rand = rand();
    z_base_a1 = D_800E2294 - 0x64;
    *(u16 *)(anim + 0x4) = 0x20C;
    anim[3] = 0x7F;
    *(u16 *)(anim + 0xA) = z_base_a1 + (z_rand % 201);
}

#include "common.h"
/* CC1_FLAGS: -fno-strength-reduce */
int *func_800C2B10(int index);
int rsin(int angle);
int rcos(int angle);

extern u8 D_800E2280;
extern u8 D_800E2281;
extern u8 D_800E2282;
extern u16 D_800E2290;
extern u16 D_800E2292;
extern u16 D_800E2294;
extern char *D_8009D254;

void FieldAnim_InitRadialEntries(void *arg0, void *arg1, u8 *anim) {
    u16 *base_s5;
    volatile int stack_pad;
    int neg_s6;
    int i;
    int angle;
    u8 *pos_entry;
    u16 *rot_entry;
    char *entity;
    char *core;
    char *mode_data;

    D_800E2280 = *func_800C2B10(3);
    D_800E2281 = *func_800C2B10(4);
    D_800E2282 = *func_800C2B10(5);

    anim[1] = 0x7F;
    entity = D_8009D254;
    core = *(char **)entity;
    mode_data = *(char **)(core + 0x68);
    if (*(short *)(mode_data + 0x6) == 3) {
        anim[2] = 8;
    } else {
        anim[2] = 0x10;
    }

    if ((s8)anim[2] > 0) {
        i = 0;
        base_s5 = &D_800E2290;
        neg_s6 = -0x1400;
        pos_entry = anim;
        rot_entry = (u16 *)anim;
        do {
            rot_entry[0x13] = base_s5[0];
            rot_entry[0x14] = base_s5[1];
            rot_entry[0x15] = base_s5[2];

            if ((s8)anim[2] == 0x10) {
                angle = i << 8;
            } else {
                angle = i << 9;
            }

            rot_entry[0x53] = rsin(angle);
            rot_entry[0x54] = neg_s6;
            rot_entry[0x55] = rcos(angle);
            *(u16 *)(pos_entry + 0x6) = 0x258;

            pos_entry += 2;
            rot_entry += 4;
        } while (++i < (s8)anim[2]);
    }
}

#include "common.h"
extern u16 D_800E2290;
extern u16 D_800E2292;
extern u16 D_800E2294;

int func_800CC244(void *arg0, void *arg1, u8 *anim) {
    u16 z;

    *(u16 *)(anim + 6) = D_800E2290;
    *(u16 *)(anim + 8) = D_800E2292;
    z = D_800E2294;
    *(u16 *)(anim + 4) = 0xBE8;
    anim[3] = 0x7F;
    *(u16 *)(anim + 0xA) = z;
}

#include "common.h"
extern u16 D_800E2290;
extern u16 D_800E2292;
extern u16 D_800E2294;

int func_800CC284(void *arg0, void *arg1, u8 *anim) {
    u16 z;

    *(u16 *)(anim + 6) = D_800E2290;
    *(u16 *)(anim + 8) = D_800E2292 - 0x64;
    z = D_800E2294;
    *(u16 *)(anim + 4) = 0;
    anim[3] = 0x7F;
    *(u16 *)(anim + 0xA) = z;
}
