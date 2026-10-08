#include "common.h"
#include "pe1/field_anim_particle.h"
#include "pe1/field_sprite_state.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/gte_types.h"
#include "pe1/gte_types.h"

int func_800CA4A8(void *arg0, u8 *state) {
    int ret = 2;

    state[1] = ret;
    return ret;
}

#include "common.h"
int func_800CA4B4(void *arg0, u8 *state, u8 *anim) {
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
int func_800CA540(void *arg0, u8 *state, u8 *anim) {
    s16 value = *(u16 *)(anim + 4) - 0x14;

    *(u16 *)(anim + 4) = value;
    if (value < 0x14) {
        *(u16 *)(anim + 4) = 0;
        state[1] = 2;
    }
}

#include "common.h"
void **FieldEng_GetSlot(void);

extern int D_800E0C88;





int func_800CA574(void) {
    int half_a2;
    register int half_a1 asm("$5");
    int shade_a0;
    register int value asm("$3");
    register void *slotData asm("$3");
    void **slot;

    slot = FieldEng_GetSlot();
    slotData = &D_800E0C88;
    *slot = slotData;

    value = 0xBD;
    D_800E2308.cell = value;
    value = 9;
    half_a2 = 0x80;
    half_a1 = 0x80;
    asm volatile("" : "=r"(half_a2), "=r"(half_a1) : "0"(half_a2), "1"(half_a1));
    D_800E2308.clut = value;
    value = 0xAE;
    D_800F34C8.cell = value;
    value = 7;
    D_800F34C8.clut = value;
    value = -0x32;
    shade_a0 = 0x50;
    asm volatile("" : "=r"(shade_a0) : "0"(shade_a0));
    D_800F34C8.offset = value;
    value = 0x68;
    D_800E2338.cell = value;
    value = -0x3C;
    D_800E2338.offset = value;
    value = 0x42;
    D_800F34E8.cell = value;
    value = 0x20;
    D_800F34E8.clut = value;
    value = 0x32;
    D_800E2308.offset = 0;
    D_800E2308.depth = half_a2;
    D_800E2308.r = half_a1;
    D_800E2308.g = half_a1;
    D_800E2308.b = half_a1;
    D_800E2308.flip = 0;
    D_800F34C8.depth = half_a2;
    D_800F34C8.r = shade_a0;
    D_800F34C8.g = shade_a0;
    D_800F34C8.b = shade_a0;
    D_800F34C8.flip = 0;
    D_800E2338.clut = 0;
    D_800E2338.depth = half_a2;
    D_800E2338.r = shade_a0;
    D_800E2338.g = shade_a0;
    D_800E2338.b = shade_a0;
    D_800E2338.flip = 0;
    D_800F34E8.offset = value;
    D_800F34E8.depth = half_a2;
    D_800F34E8.r = half_a1;
    D_800F34E8.g = half_a1;
    D_800F34E8.b = half_a1;
    D_800F34E8.flip = 0;

    return 0;
}

extern void FieldEng_Spawn6(int a, int b, int c, int d, int e, int f);

int func_800CA6D4(int a, int b, int c, int d, int e, int f) {
    FieldEng_Spawn6(a, b, c, d, e, f);
    return 0;
}

extern char D_800E0B84[];
extern void FieldEng_Register(void *o, void *table);

int func_800CA700(void *o) {
    FieldEng_Register(o, D_800E0B84);
    return 0;
}

extern char D_800E0BA0[];
extern char D_800E0B68[];
extern char D_800E0BBC[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800CA798(char *obj);

int func_800CA728(void *obj) {
    int first;
    int second;
    int status;

    first = func_800C251C(obj, D_800E0BA0);
    second = func_800C2758(obj, D_800E0B68, D_800E0BBC);
    status = first | second;
    if (status == -1) {
        func_800CA798(obj);
    }
    return 0;
}

int func_800CA798(char *arg0) {
    *arg0 = 4;
    return 0;
}

int func_800CA7A8(void) {
    return 0;
}

int func_800CA7B0(void) {
    return 0;
}

#include "common.h"
void func_800CEDA8(int arg0);

extern char *D_800E27A8;

int func_800CA7B8(char *obj) {
    char *data = *(char **)(obj + 8);

    D_800E2360.x = *(int *)(*(char **)(data + 0x238) + 0x274);
    D_800E2360.y = *(int *)(*(char **)(data + 0x238) + 0x278);
    D_800E2360.z = *(int *)(*(char **)(data + 0x238) + 0x27C);
    D_800E27A8 = data;
    func_800CEDA8(0);
}

#include "common.h"

int rand(void);


int func_800CA824(void *arg0, void *arg1, FieldAnimMovingParticle *anim) {
    GteShortVector vec;

    vec.x = -((rand() % 3) + 9);
    vec.y = -((rand() % 3) + 9);
    vec.z = (rand() % 5) - 2;

    ApplyMatrixSV(*(void **)(D_800E27A8 + 0x238), &vec, &anim->velocity);

    {
        u16 z;

        anim->position.x = D_800E2360.x;
        anim->position.y = D_800E2360.y;
        z = D_800E2360.z;
        anim->lifetime = 0x14;
        anim->age = 0;
        anim->position.z = z;
    }
}

#include "common.h"

extern char *D_8009D254;
extern char D_800E0C08[];

int func_800CA934(void *arg0, void *arg1, FieldAnimMatrixGlow *anim) {
    FieldAnimMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    char *data;
    char *entry;
    char *model;

    entry = D_800E0C08 + (((short)(*(u16 *)(*(char **)(*(char **)D_8009D254 + 0x68) + 6) - 1)) << 3);
    ApplyMatrixSV(*(char **)(D_800E27A8 + 0x238) + 0x260, entry, &out);

    anim_s0->position.x = D_800E2360.x + out.x;
    anim_s0->position.y = D_800E2360.y + out.y;
    data = D_800E27A8;
    anim_s0->position.z = D_800E2360.z + out.z;

    model = *(char **)(data + 0x238);
    anim_s0->transform = *(GteMatrixStorage *)(model + 0x260);
    anim_s0->brightness = 0x7F;
}

#include "common.h"

extern GteShortVector D_800C21C4;
extern GteShortVector D_800C21CC;

int func_800CAA38(void *arg0, void *arg1, FieldAnimTwinGlow *anim) {
    FieldAnimTwinGlow *anim_s0;
    u16 *field_s1;
    GteShortVector *out_s2;
    u16 z_base_v1;
    u16 z_out_a0;
    GteShortVector in0;
    GteShortVector in1;
    GteShortVector out;
    char *data;

    anim_s0 = anim;

    in0 = D_800C21C4;
    in1 = D_800C21CC;
    out_s2 = &out;

    ApplyMatrixSV(*(char **)(D_800E27A8 + 0x238) + 0x260, &in0, out_s2);
    field_s1 = &D_800E2360.x;
    anim_s0->position[0].x = field_s1[0] + out.x;
    anim_s0->position[0].y = D_800E2360.y + out.y;
    data = D_800E27A8;
    anim_s0->position[0].z = D_800E2360.z + out.z;

    ApplyMatrixSV(*(char **)(data + 0x238) + 0x260, &in1, out_s2);
    anim_s0->position[1].x = field_s1[0] + out.x;
    anim_s0->position[1].y = D_800E2360.y + (u16)out.y;
    z_base_v1 = D_800E2360.z;
    z_out_a0 = out.z;
    anim_s0->brightness = 0x7F;
    anim_s0->scale = 0;
    anim_s0->position[1].z = z_base_v1 + z_out_a0;
}

#include "common.h"

int func_800CAB88(void *arg0, void *arg1, u8 *anim) {
    u16 x = D_800E2360.x;
    char *data = D_8009D254;
    int y;
    u16 z;

    *(u16 *)(anim + 0x8) = x;
    y = *(s16 *)(data + 0x2E);
    *(u16 *)(anim + 0xA) = y;
    z = D_800E2360.z;
    *(u16 *)(anim + 0x4) = 0x7F;
    *(u16 *)(anim + 0x6) = 0x224;
    *(u16 *)(anim + 0xC) = z;
}

#include "common.h"

extern char D_800E0C48[];

int func_800CABC8(void *arg0, void *arg1, FieldAnimMatrixGlow *anim) {
    FieldAnimMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    char *data;
    char *entry;
    char *model;

    entry = D_800E0C48 + (((short)(*(u16 *)(*(char **)(*(char **)D_8009D254 + 0x68) + 6) - 1)) << 3);
    ApplyMatrixSV(*(char **)(D_800E27A8 + 0x238) + 0x260, entry, &out);

    anim_s0->position.x = D_800E2360.x + out.x;
    anim_s0->position.y = D_800E2360.y + out.y;
    data = D_800E27A8;
    anim_s0->position.z = D_800E2360.z + out.z;

    model = *(char **)(data + 0x238);
    anim_s0->transform = *(GteMatrixStorage *)(model + 0x260);
    anim_s0->brightness = 0x7F;
    anim_s0->scale = 0x224;
}

void func_800CACD4(void) {
}

#include "common.h"

void *memset(void *dest, int value, unsigned int count);

extern s16 D_800E0BE8[];

int func_800CACDC(void *arg0, void *arg1, u8 *anim) {
    GteMatrix matrix;
    GteShortVector rot;
    GteVector scaleCopy;
    GteVector scale;
    s16 index;

    index = *(u16 *)(*(char **)(*(char **)D_8009D254 + 0x68) + 6) - 1;

    rot.x = 0;
    rot.y = 0;
    rot.z = (s8)anim[1] << 6;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x10, 0x10);
    func_800C3238(0);

    RotMatrix(&rot, &matrix);

    matrix.t[0] = *(s16 *)(anim + 0x8);
    matrix.t[1] = *(s16 *)(anim + 0xA);
    matrix.t[2] = *(s16 *)(anim + 0xC);

    memset(&scale, 0, sizeof(scale));
    scale.x = D_800E0BE8[index];
    scale.y = D_800E0BE8[index];
    scale.z = D_800E0BE8[index];
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    func_800C42A4(&D_800E2308, &matrix, 1);
}
