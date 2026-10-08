#include "common.h"
#include "pe1/field_engine_slot.h"
#include "pe1/field_anim_particle.h"
#include "pe1/field_sprite_state.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/gte_types.h"

void *memset(void *dest, int value, unsigned int count);

extern u16 D_800E2332;

int func_800C9868(void *arg0, void *arg1, FieldAnimGlowPoint *anim) {
    u16 *field_s3 = &D_800E2332;
    GteMatrix matrix;
    int scale;
    u16 field;
    int localScale[4];

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    scale = anim->scale + 0x170;
    field = anim->brightness;

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

    *field_s3 = field;
    matrix.t[0] = (s16)anim->position.x;
    matrix.t[1] = (s16)anim->position.y;
    matrix.t[2] = (s16)anim->position.z;

    memset(localScale, 0, sizeof(localScale));
    localScale[0] = scale;
    localScale[1] = scale;
    localScale[2] = scale;

    ScaleMatrix(&matrix, (const GteVector *)localScale);
    func_800C42A4((FieldGlowSprite *)((u8 *)field_s3 - 10), &matrix, 1);
}

int func_800C9968(void *arg0, u8 *state) {
    int ret = 2;

    state[1] = ret;
    return ret;
}
int func_800C9974(void *arg0, FieldEngSlot *state, FieldAnimMovingParticle *anim) {
    FieldEngSlot *state_a3 = state;
    FieldAnimMovingParticle *anim_a2 = anim;
    int temp_v0;
    int temp_v1;
    register int temp_a0 asm("$4");
    register int temp_a1 asm("$5");
    u8 count;

    temp_v0 = anim_a2->position.x;
    temp_v1 = (u16)anim_a2->velocity.x;
    temp_a0 = (u16)anim_a2->velocity.y;
    temp_a1 = (u16)anim_a2->velocity.z;

    temp_v0 += temp_v1;
    anim_a2->position.x = temp_v0;
    temp_v0 = anim_a2->position.y;
    temp_v1 = anim_a2->position.z;
    temp_v0 += temp_a0;
    temp_v1 += temp_a1;
    anim_a2->position.y = temp_v0;
    temp_v0 = (u16)anim_a2->velocity.y;
    temp_a0 = (unsigned int)anim_a2;
    anim_a2->position.z = temp_v1;
    temp_v1 = anim_a2->age;
    temp_v0 += 3;
    anim_a2->velocity.y = temp_v0;
    temp_v0 = (s16)anim_a2->position.y;
    temp_v1++;
    anim_a2->age = temp_v1;
    if (temp_v0 > 0) {
        anim_a2->velocity.y = -(u16)anim_a2->velocity.y;
    }
    count = ((FieldAnimMovingParticle *)temp_a0)->lifetime;
    ((FieldAnimMovingParticle *)temp_a0)->lifetime = count - 1;
    if (count == 0) {
        state_a3->flag = 2;
    }
}
int func_800C9A00(void *arg0, FieldEngSlot *state, FieldAnimGlowPoint *anim) {
    s16 value = anim->brightness - 0x14;

    anim->brightness = value;
    if (value < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}
int func_800C9A34(void *arg0, FieldEngSlot *state, FieldAnimGlowPoint *anim) {
    anim->brightness = anim->brightness - 8;
    anim->scale = (u16)anim->scale + 0x28;
    if ((s16)anim->brightness < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}

extern int D_800E0B38;



int func_800C9A70(char *object) {
    int half;
    register int value asm("$3");
    register void *slotData asm("$3");
    void **slot;

    slot = FieldEng_GetSlot(object);
    slotData = &D_800E0B38;
    *slot = slotData;

    value = 0xBD;
    D_800E22F8.cell = value;
    value = 9;
    half = 0x80;
    asm volatile("" : "=r"(half) : "0"(half));
    D_800E22F8.clut = value;
    value = 0x80;
    D_800E22F8.r = value;
    D_800E22F8.g = value;
    D_800E22F8.b = value;
    value = 0xAC;
    D_800F34B8.cell = value;
    value = 6;
    D_800F34B8.clut = value;
    value = -0x32;
    D_800F34B8.offset = value;
    value = 0x50;
    D_800E22F8.offset = 0;
    D_800E22F8.depth = half;
    D_800E22F8.flip = 0;
    D_800F34B8.depth = half;
    D_800F34B8.r = value;
    D_800F34B8.g = value;
    D_800F34B8.b = value;
    D_800F34B8.flip = 0;

    return 0;
}
extern void FieldEng_Spawn6(int a, int b, int c, int d, int e, int f);

int func_800C9B3C(int a, int b, int c, int d, int e, int f) {
    FieldEng_Spawn6(a, b, c, d, e, f);
    return 0;
}
extern char D_800E0A94[];
extern void FieldEng_Register(void *o, void *table);

int func_800C9B68(void *o) {
    FieldEng_Register(o, D_800E0A94);
    return 0;
}
extern char D_800E0AA4[];
extern char D_800E0A84[];
extern char D_800E0AB4[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800C9C00(char *obj);

int func_800C9B90(void *obj) {
    int first;
    int second;
    int status;

    first = func_800C251C(obj, D_800E0AA4);
    second = func_800C2758(obj, D_800E0A84, D_800E0AB4);
    status = first | second;
    if (status == -1) {
        func_800C9C00(obj);
    }
    return 0;
}
int func_800C9C00(char *arg0) {
    *arg0 = 4;
    return 0;
}
int func_800C9C10(void) {
    return 0;
}
int func_800C9C18(void) {
    return 0;
}
void func_800CEDA8(int arg0);

extern char *D_800E27A4;

int func_800C9C20(char *obj) {
    char *data = *(char **)(obj + 8);

    D_800E2358.x = *(int *)(*(char **)(data + 0x238) + 0x274);
    D_800E2358.y = *(int *)(*(char **)(data + 0x238) + 0x278);
    D_800E2358.z = *(int *)(*(char **)(data + 0x238) + 0x27C);
    D_800E27A4 = data;
    func_800CEDA8(0);
}

int rand(void);


int func_800C9C8C(void *arg0, void *arg1, FieldAnimMovingParticle *anim) {
    GteShortVector vec;
    int (*model)[];

    vec.x = -(rand() % 3 + 9);
    vec.y = -(rand() % 3 + 9);
    vec.z = rand() % 5 - 2;

    model = *(int (**)[])(D_800E27A4 + 0x238);
    ApplyMatrixSV(model, &vec, &anim->velocity);

    anim->position.x = D_800E2358.x;
    anim->position.y = D_800E2358.y;
    anim->position.z = D_800E2358.z;
    anim->lifetime = 0x14;
    anim->age = 0;
}

extern char *D_8009D254;
extern char D_800E0AF8[];

int func_800C9D9C(void *arg0, void *arg1, FieldAnimMatrixGlow *anim) {
    FieldAnimMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    char *data;
    char *entry;
    char *model;

    entry = D_800E0AF8 + (((short)(*(u16 *)(*(char **)(*(char **)D_8009D254 + 0x68) + 6) - 1)) << 3);
    ApplyMatrixSV(*(char **)(D_800E27A4 + 0x238) + 0x260, entry, &out);

    anim_s0->position.x = D_800E2358.x + out.x;
    anim_s0->position.y = D_800E2358.y + out.y;
    data = D_800E27A4;
    anim_s0->position.z = D_800E2358.z + out.z;

    model = *(char **)(data + 0x238);
    anim_s0->transform = *(GteMatrixStorage *)(model + 0x260);
    anim_s0->brightness = 0x7F;
}

void func_800C9EA0(void) {
}

void *memset(void *dest, int value, unsigned int count);

extern s16 D_800E0AD8[];

int func_800C9EA8(void *arg0, void *arg1, u8 *anim) {
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
    scale.x = D_800E0AD8[index];
    scale.y = D_800E0AD8[index];
    scale.z = D_800E0AD8[index];
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    func_800C42A4(&D_800E22F8, &matrix, 1);
}
