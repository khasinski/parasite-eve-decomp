#include "common.h"
#include "pe1/field_engine_slot.h"
#include "pe1/field_glow_sprite.h"

extern int D_800E0928;

int func_800C7BA0(char *object) {
    int half_a2;
    u8 fullBrightness;
    int shade_a0;
    register int value asm("$3");
    register void *slotData asm("$3");
    void **slot;

    slot = FieldEng_GetSlot(object);
    slotData = &D_800E0928;
    *slot = slotData;

    value = 0xBD;
    D_800E22D8.cell = value;
    value = 9;
    half_a2 = 0x80;
    fullBrightness = 0x80;
    asm volatile("" : "=r"(half_a2), "=r"(fullBrightness) : "0"(half_a2), "1"(fullBrightness));
    D_800E22D8.clut = value;
    value = 0xAE;
    D_800F3498.cell = value;
    value = 7;
    D_800F3498.clut = value;
    value = -0x32;
    shade_a0 = 0x50;
    asm volatile("" : "=r"(shade_a0) : "0"(shade_a0));
    D_800F3498.offset = value;
    value = 0x68;
    D_800E2318.cell = value;
    value = -0x3C;
    D_800E2318.offset = value;
    value = 0x42;
    D_800F34D8.cell = value;
    value = 0x20;
    D_800F34D8.clut = value;
    value = 0x32;
    D_800E22D8.offset = 0;
    D_800E22D8.depth = half_a2;
    D_800E22D8.r = fullBrightness;
    D_800E22D8.g = fullBrightness;
    D_800E22D8.b = fullBrightness;
    D_800E22D8.flip = 0;
    D_800F3498.depth = half_a2;
    D_800F3498.r = shade_a0;
    D_800F3498.g = shade_a0;
    D_800F3498.b = shade_a0;
    D_800F3498.flip = 0;
    D_800E2318.clut = 0;
    D_800E2318.depth = half_a2;
    D_800E2318.r = shade_a0;
    D_800E2318.g = shade_a0;
    D_800E2318.b = shade_a0;
    D_800E2318.flip = 0;
    D_800F34D8.offset = value;
    D_800F34D8.depth = half_a2;
    D_800F34D8.r = fullBrightness;
    D_800F34D8.g = fullBrightness;
    D_800F34D8.b = fullBrightness;
    D_800F34D8.flip = 0;

    return 0;
}
