#include "pe1/akao/commands.h"
/* Burst and spark effect setup, draw and lifecycle callbacks. */

#include "common.h"
#include "pe1/field_sprite_state.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/field_anim.h"
#include "pe1/field_anim_particle.h"

extern u16 D_800E27EA;

int func_800CD50C(void *arg0, void *arg1, FieldAnimGlowPoint *anim) {
    int *base_a1;
    u16 *field_v1;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(3);

    base_a1 = &D_800F3478.t[0];
    field_v1 = &D_800E27EA;
    base_a1[0] = (s16)anim->position.x;
    D_800F3478.t[1] = (s16)anim->position.y;
    D_800F3478.t[2] = (s16)anim->position.z;
    *field_v1 = anim->brightness;
    func_800C42A4((FieldGlowSprite *)((u8 *)field_v1 - 10), (GteMatrix *)(base_a1 - 5), 1);
}

void func_800CD59C(void) {
}

int func_800CD5A4(void *arg0, u8 *state) {
    int ret = 2;
    state[1] = ret;
    return ret;
}

int func_800CD5B0(void *arg0, u8 *state, FieldAnimGlowPoint *anim) {
    anim->brightness = anim->brightness - 0xA;
    (*(u16 *)&anim->scale) = (*(u16 *)&anim->scale) + 0x3C;
    if ((s16)anim->brightness < 0x14) {
        anim->brightness = 0;
        state[1] = 2;
    }
}

int func_800CD5EC(void *arg0, u8 *state, FieldAnimTwoPointMotion *anim) {
    unsigned int i = 0;

    do {
        unsigned slot = i & 0xFFFF;

        i++;
        anim->position[slot].x = anim->position[slot].x + anim->velocity[slot].x;
        anim->position[slot].y = anim->position[slot].y + anim->velocity[slot].y;
        anim->position[slot].z = anim->position[slot].z + anim->velocity[slot].z;
    } while ((i & 0xFFFF) < 2);

    anim->age++;
    if ((signed char)anim->age >= 8) {
        state[1] = 2;
    }
}

int rand(void);

int func_800CD678(void *arg0, u8 *state, FieldAnimGlowPoint *anim) {
    int value = rand() % 11;
    int jitter;
    FieldAnimGlowPoint *output;
    u16 positionY = anim->position.y;
    int brightness = anim->brightness;
    int positionX = anim->position.x;
    positionY -= 8;
    brightness -= 8;
    output = anim;
    positionX -= 5;
    output->position.y = positionY;
    output->brightness = brightness;
    jitter = value;
    value = 8;
    positionX += jitter;
    anim->position.x = positionX;
    if (value > (short)brightness) {
        state[1] = 2;
    }
}

int func_800CD71C(void *arg0, u8 *state) {
    int ret = 2;
    state[1] = ret;
    return ret;
}

#include "pe1/field_engine_slot.h"

extern int D_800E0F6C;




int func_800CD728(char *object) {
    void **slot = FieldEng_GetSlot(object);
    int value;
    register int byte2 asm("$4");

    value = (int)&D_800E0F6C;
    *slot = (void *)value;

    value = 0x5F4;
    D_800F33E8.oriented.scale.x = value;
    D_800F33E8.oriented.scale.y = value;
    D_800F33E8.oriented.scale.z = value;

    value = 0x40;
    byte2 = 0x20;
    D_800F33E8.oriented.cell = value;
    value = -0x64;
    D_800F33E8.oriented.depth = value;

    value = 0x15;
    D_800F33E8.oriented.rgb[0] = value;

    value = 0x46;
    D_800E27B0.oriented.cell = value;

    value = 0x30;
    D_800E27B0.oriented.clut = value;

    value = -0x6E;
    D_800E27B0.oriented.depth = value;

    value = 0xFF;
    D_800E27B0.oriented.rgb[0] = value;

    value = 0xB0;
    D_800E27B0.oriented.rgb[1] = value;
    D_800E27B0.oriented.rgb[2] = value;

    value = 0xA4;
    D_800E2770.oriented.scale.x = value;
    D_800E2770.oriented.scale.y = value;

    value = 0x108;
    D_800E2770.oriented.scale.z = value;

    value = 0x6E;
    D_800E2770.oriented.cell = value;

    value = 3;
    D_800E2770.oriented.clut = value;

    value = -0x96;
    D_800E2770.oriented.depth = value;

    value = 0x80;
    D_800F33E8.oriented.rotation.x = 0;
    D_800F33E8.oriented.rotation.y = 0;
    D_800F33E8.oriented.rotation.z = 0;
    D_800F33E8.oriented.clut = byte2;
    D_800F33E8.oriented.rgb[1] = byte2;
    D_800F33E8.oriented.rgb[2] = byte2;
    D_800E27B0.oriented.rotation.x = 0;
    D_800E27B0.oriented.rotation.y = 0;
    D_800E27B0.oriented.rotation.z = 0;
    D_800E2770.oriented.rotation.x = 0;
    D_800E2770.oriented.rotation.y = 0;
    D_800E2770.oriented.rotation.z = 0;
    D_800E2770.oriented.rgb[0] = value;
    D_800E2770.oriented.rgb[1] = value;
    D_800E2770.oriented.rgb[2] = value;

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

int *func_800C2B10(int index);
int *func_800C2B28(int index);
void func_800CEDA8(int arg0);

extern int D_800E2804;
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
    D_800E27F0.x = *(u16 *)(data + 0x268);
    D_800E27F0.y = *(u16 *)(data + 0x26A);
    D_800E27F0.z = *(u16 *)(data + 0x26C);

    if (D_800B0CE8 != 0) {
        Akao_Cmd_24((void *)D_800B0E14, 0, 0x80, 0x7F);
    }

    func_800CEDA8(0);
}

int func_800CDA5C(void *arg0, void *arg1, FieldAnimSparkPoint *anim) {
    u16 *base = &D_800E27F0.x;
    u16 *base_s4;
    int i = 0;
    u8 *entry;
    int value;

    anim->position.x = base[0];
    anim->position.y = D_800E27F0.y;
    anim->position.z = D_800E27F0.z;
    anim->displacement.x = (rand() % 201) - 0x64;
    anim->displacement.y = (rand() % 101) - 0x32;

    base_s4 = base;
    anim->brightness = 0x7F;
    anim->displacement.z = 0;
    anim->scaleStep = 0;

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

int func_800CDC24(void *arg0, void *arg1, FieldAnimSparkPoint *anim) {
    int value;

    anim->position.x = D_800E27F0.x;
    anim->position.y = D_800E27F0.y;
    anim->position.z = D_800E27F0.z;
    anim->brightness = 0x7F;
    anim->displacement.x = (rand() % 201) - 0x64;
    value = rand();
    anim->displacement.z = 0;
    anim->displacement.y = (value % 101) - 0x32;
}

void func_800CDD04(void) {
}

#include "pe1/field_billboard.h"


int func_800CDD0C(void *arg0, void *arg1, FieldAnimSparkPoint *anim) {
    u16 lhs_v0;
    FieldBillboard *output;
    u16 rhs_v1;
    unsigned int i;
    u8 *entry;

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    lhs_v0 = anim->position.x;
    rhs_v1 = anim->displacement.x;
    output = &D_800E27B0.billboard;
    lhs_v0 += rhs_v1;
    output->position.x = lhs_v0;
    D_800E27B0.oriented.position.y = anim->position.y + anim->displacement.y;
    D_800E27B0.oriented.position.z = anim->position.z + anim->displacement.z;
    D_800E27B0.oriented.scale.x = ((s8)anim->scaleStep * 8) + 0x20C;
    D_800E27B0.oriented.scale.y = ((s8)anim->scaleStep * 8) + 0x20C;
    D_800E27B0.oriented.scale.z = ((s8)anim->scaleStep * 8) + 0x20C;
    D_800E27B0.oriented.brightness = (s8)anim->brightness;

    func_800C3B04(output);
    func_800C3098(0x10);

    i = 0;
    do {
        entry = (u8 *)(((i & 0xFFFF) * 2) + (int)anim);
        D_800E2770.billboard.position.x = *(u16 *)(entry + 0x10) + anim->displacement.x;
        D_800E2770.billboard.position.y = *(u16 *)(entry + 0x20) + anim->displacement.y;
        D_800E2770.billboard.position.z = *(u16 *)(entry + 0x30) + anim->displacement.z;
        i++;
        D_800E2770.billboard.brightness = (s8)anim->brightness;
        func_800C3B04(&D_800E2770.billboard);
    } while ((i & 0xFFFF) < 8);
}


int func_800CDE90(void *arg0, void *arg1, FieldAnimSparkPoint *anim) {
    u16 lhs_v0;
    FieldBillboard *output;
    u16 rhs_v1;

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(3);

    lhs_v0 = anim->position.x;
    rhs_v1 = anim->displacement.x;
    output = &D_800F33E8.billboard;
    lhs_v0 += rhs_v1;
    output->position.x = lhs_v0;
    D_800F33E8.oriented.position.y = anim->position.y + anim->displacement.y;
    D_800F33E8.oriented.position.z = anim->position.z + anim->displacement.z;
    D_800F33E8.oriented.brightness = (s8)anim->brightness;
    func_800C3B04(output);
}

int func_800CDF40(void *arg0, u8 *state) {
    int ret = 2;
    state[1] = ret;
    return ret;
}

int func_800CDF4C(void *arg0, u8 *state, FieldAnimSparkPoint *anim) {
    unsigned int i = 0;

    anim->brightness -= 0x10;
    anim->scaleStep += 0xC;

    do {
        u8 *entry = (u8 *)(((i & 0xFFFF) * 2) + (int)anim);

        i++;
        *(u16 *)(entry + 0x10) = *(u16 *)(entry + 0x10) + *(u16 *)(entry + 0x40);
        *(u16 *)(entry + 0x20) = *(u16 *)(entry + 0x20) + *(u16 *)(entry + 0x50);
        *(u16 *)(entry + 0x30) = *(u16 *)(entry + 0x30) + *(u16 *)(entry + 0x60);
    } while ((i & 0xFFFF) < 8);

    if ((signed char)anim->brightness < 0x10) {
        state[1] = 2;
    }
}

int func_800CDFE0(void *arg0, u8 *state, FieldAnimSparkPoint *anim) {
    int value = rand() % 11;
    int jitter;
    FieldAnimSparkPoint *output;
    u16 positionY = anim->position.y;
    int brightness = anim->brightness;
    int positionX = anim->position.x;
    positionY -= 4;
    brightness -= 3;
    output = anim;
    positionX -= 5;
    output->position.y = positionY;
    output->brightness = brightness;
    jitter = value;
    value = 3;
    positionX += jitter;
    anim->position.x = positionX;
    if (value > (signed char)brightness) {
        state[1] = 2;
    }
}

extern int D_800E0FFC;
extern int D_800E22B8;
extern int D_800E22BC;
extern int D_800E22C0;
extern short D_800E22B0;
extern short D_800E22B2;
extern short D_800E22B4;
extern u8 D_800E22C8;
extern u8 D_800E22C9;
extern u8 D_800E22CA;
extern u8 D_800E22CD;
extern short D_800E22CE;
int func_800CE084(char *object)
{
  void **slot = FieldEng_GetSlot(object);
  register int value;
  register void *slotData;
  slotData = &D_800E0FFC;
 do { *slot = slotData; value = 0x300; D_800E22B8 = value; D_800E22BC = value; } while (0);
  D_800E22C0 = value;
  value = 5;
  D_800E22CD = value;
  value = -0x64;
  D_800E22CE = value;
  value = 0x80;
  D_800E22B0 = 0;
  D_800E22B2 = 0;
  D_800E22B4 = 0;
  D_800E22C8 = value;
  D_800E22C9 = value;
  D_800E22CA = value;
  return 0;
}

int func_800CE118(int a, int b, int c, int d, int e, int f) {
    FieldEng_Spawn6(a, b, c, d, e, f);
    return 0;
}

extern char D_800E0FC0[];

int func_800CE144(void *o) {
    FieldEng_Register(o, D_800E0FC0);
    return 0;
}

extern char D_800E0FCC[];
extern char D_800E0FB4[];
extern char D_800E0FD8[];
extern int func_800CE1DC(char *arg0);

int func_800CE16C(void *obj) {
    int first;
    int second;
    int status;

    first = func_800C251C(obj, D_800E0FCC);
    second = func_800C2758(obj, D_800E0FB4, D_800E0FD8);
    status = first | second;
    if (status == -1) {
        func_800CE1DC(obj);
    }
    return 0;
}

int func_800CE1DC(char *arg0) {
    *arg0 = 4;
    return 0;
}

int func_800CE1EC(void) {
    return 0;
}

int func_800CE1F4(void) {
    return 0;
}

int *func_800C2B10(int index);
int *func_800C2B28(int index);
void func_800CEDA8(int arg0);

extern int D_800E2848;

int func_800CE1FC(void) {
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
    D_800E2848 = (int)entry;

    if (next == 0) {
        *func_800C2B10(0xE) = 1;
    }

    *func_800C2B10(0xD) = index;

    data = (u8 *)D_800E2848;
    D_800E2808.x = *(u16 *)(data + 0x268);
    D_800E2808.y = *(u16 *)(data + 0x26A);
    D_800E2808.z = *(u16 *)(data + 0x26C);

    func_800CEDA8(0);
}

int func_800CE2B4(void *arg0, void *arg1, FieldAnimTimedSpark *anim) {
    int randomX;
    int randomY;
    u16 base;
    int randomZ;

    randomX = rand();
    base = D_800E2808.x - 0x28;
    anim->position.x = base + (randomX % 80);

    randomY = rand();
    base = D_800E2808.y - 0x28;
    anim->position.y = base + (randomY % 80);

    randomZ = rand();
    base = D_800E2808.z;

    anim->brightness = 0x7F;
    anim->reserved04 = 0;
    anim->age = 0;
    base -= 0x28;
    anim->position.z = base + (randomZ % 80);
}

void func_800CE3AC(void) {
}

extern u16 D_800E22A8;
extern u16 D_800E22AA;
extern u16 D_800E22AC;
extern short D_800E22D0;
extern signed char D_800E22CC;

int func_800CE3B4(void *arg0, void *arg1, u8 *anim) {
    u16 value_v0;
    FieldBillboard *output;
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    value_v0 = *(u16 *)(anim + 0x6);
    output = (FieldBillboard *)&D_800E22A8;
    output->position.x = value_v0;
    D_800E22AA = *(u16 *)(anim + 0x8);
    D_800E22AC = *(u16 *)(anim + 0xA);
    D_800E22D0 = (signed char)anim[1];
    D_800E22CC = (anim[3] * 2) - 0x60;

    func_800C3B04(output);
    func_800C3098(0x10);
}

int func_800CE464(void *arg0, char *arg1) {
    int value;

    value = 2;
    arg1[1] = value;
    return value;
}

void func_800CE470(void *arg0, char *arg1, FieldAnimTimedSpark *anim) {
    anim->age++;
    if ((signed char)anim->age == 6) {
        arg1[1] = 2;
    }
}
