#include "pe1/game_state.h"
#include "pe1/scene_assets.h"
#include "common.h"

void Akao_ClearVoiceBank(void)
{
    register u8 *base asm("$6") = (u8 *)&g_GameState;
    register u8 *p asm("$4");
    register int i asm("$5");
    register int secondMinusOne asm("$4");
    register int minusOne asm("$3");
    register int shortMinusOne asm("$3");
    register u8 *q asm("$3");
    register SceneBankResetPair *pair asm("$5");
    register int pairOffset asm("$4");
    register int j asm("$5");
    register u8 flags asm("$2");
    register int fill asm("$3");
    register u32 voiceBase asm("$3");
    register u8 *tailPtr asm("$7");
    register u8 *tailPtr2 asm("$2");
    register u32 drawMode asm("$4");
    i = 0;
    p = base;
    PE1_COMPILER_USE(i);
    PE1_COMPILER_USE(p);
    ((Pe1GameState *)base)->flags = 3;
    *(u16 *)(base + 4) = 10;
    *(s16 *)(base + 6) = -1;
    base[8] = 2;
    PE1_COMPILER_LAUNDER_AFTER_MEM(minusOne, -1, base[8]);
    base[10] = 11;
    base[9] = minusOne;
    base[11] = 0;
    base[12] = minusOne;
    base[13] = 1;
    base[14] = 0;
    base[15] = 0;
    base[16] = 0;
    ((Pe1GameState *)base)->bank_state_11 = 0;
    ((Pe1GameState *)base)->bank_state_12 = 0;
    base[19] = 0;
    for (; (unsigned int)i < 49; i++, p += 4)
        *(u32 *)(p + 0x14) = 0;
    base[0xD9] = 8;
    base[0xD8] = 0;
    *(s8 *)(base + 0xDB) = -1;
    *(s8 *)(base + 0xDA) = -1;
    j = 0;
    secondMinusOne = -1;
    q = base;
    do {
        q[0xDD] = secondMinusOne;
        q[0xDC] = secondMinusOne;
        PE1_COMPILER_LAUNDER_AFTER_MEM(j, j, q[0xDC]);
        j++;
        q += 2;
    } while (j < 2);
    pair = D_80094488;
    pairOffset = 0;
    PE1_COMPILER_USE(pair);
    PE1_COMPILER_USE(pairOffset);
    base[0xFF] = 0x7F;
    base[0xFE] = 0x7F;
    base[0xE0] = 0x27;
    base[0xE1] = 0x0D;
    base[0xE3] = 1;
    PE1_COMPILER_LAUNDER_AFTER_MEM(shortMinusOne, -1, base[0xE3]);
    base[0xE6] = 0x98;
    base[0xE2] = 0;
    *(s16 *)(base + 0xE4) = shortMinusOne;
    *(s8 *)(base + 0xE7) = -1;
    *(s16 *)(base + 0xE8) = shortMinusOne;
    base[0xEB] = 0;
    base[0xEA] = 0;
    for (; pairOffset < 32; pairOffset += 8, pair++) {
        pair->second = 0;
        *(u16 *)((u8 *)D_80094488 + 4 + pairOffset) = 0;
    }
    drawMode = 0xE1000440;
    PE1_COMPILER_USE(drawMode);
    i = 2;
    PE1_COMPILER_USE(i);
    ((Pe1GameState *)base)->bank_value_f6 = 0x30;
    ((Pe1GameState *)base)->bank_value_f7 = 0x7F;
    ((Pe1GameState *)base)->bank_value_f8 = 0x100;
    ((Pe1GameState *)base)->bank_value_fa = 0x800;
    base[0x107] = 3;
    base[0x10B] = 0x60;
    *(u16 *)(base + 0x110) = 0x140;
    *(u16 *)(base + 0x112) = 0xE0;
    base[0x117] = 1;
    PE1_COMPILER_MEMORY_BARRIER();
    flags = base[0x10B];
    PE1_COMPILER_MEMORY_BARRIER();
    fill = 0x20;
    base[0xED] = fill;
    base[0x108] = fill;
    base[0x109] = fill;
    base[0x10A] = fill;
    voiceBase = ((Pe1GameState *)base)->voice_bank_base;
    tailPtr = base + 8;
    base[0xEC] = 0;
    base[0xEE] = 0;
    base[0xEF] = 0;
    base[0xF0] = 0;
    base[0xF1] = 0;
    base[0xF4] = 0;
    base[0xF2] = 0;
    base[0xF4] = 0;
    *(u16 *)(base + 0x10C) = 0;
    *(u16 *)(base + 0x10E) = 0;
    *(u32 *)(base + 0x118) = drawMode;
    *(u32 *)(base + 0x11C) = 0;
    *(u32 *)(base + 0x120) = 0;
    *(u32 *)(base + 0x124) = 0;
    base[0x10B] = flags | 2;
    ((Pe1GameState *)base)->bank_work_base = voiceBase;
    ((Pe1GameState *)base)->bank_work_end = voiceBase + 0x1400;
    ((Pe1GameState *)base)->bank_work_far_end = voiceBase + 0x2800;
    for (; i >= 0; i--, tailPtr -= 4)
        *(u32 *)(tailPtr + 0x134) = 0;
    for (i = 1, tailPtr2 = base + 4; i >= 0; i--, tailPtr2 -= 4)
        *(u32 *)(tailPtr2 + 0x140) = 0;
    *(u32 *)(base + 0x148) = 0;
}
