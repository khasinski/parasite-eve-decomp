#include "pe1/cdrom.h"
#include "pe1/scene_assets.h"
#include "pe1/game_state.h"
#include "common.h"
#include "pe1/cdrom_buffers.h"

void func_80085644(void);
void Akao_Cmd_F0(void);
void Akao_Cmd_F1(void);
void Akao_Cmd_98_9A_9C(int arg0);
void VSync(int mode);

extern s16 D_800B0DD4;

void func_8006A5BC(void)
{
    func_80085644();
    Akao_Cmd_F0();
    Akao_Cmd_F1();
    Akao_Cmd_98_9A_9C(0);

    while (DsReset() != 1) {
        VSync(0);
    }

    while (DsSystemStatus() != 1) {
        VSync(0);
    }

    D_800B0DD4 = DsShellOpen();
}

void Boot_InitMemoryLayout(void);

/* Lay out the memory buffers, then reset the scene voice-bank state. */
void Boot_InitGameState(void)
{
    Boot_InitMemoryLayout();
    Akao_ClearVoiceBank();
}

void Akao_ClearVoiceBank(void)
{
    u8 *base = (u8 *)&g_GameState;
    u8 *p;
    int i;
    int secondMinusOne;
    int minusOne;
    int shortMinusOne;
    register u8 *q asm("$3");
    SceneBankResetPair *pair;
    int pairOffset;
    register int j asm("$5");
    u8 flags;
    register int fill asm("$3");
    u32 voiceBase;
    u8 *tailPtr;
    u8 *tailPtr2;
    u32 drawMode;
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

extern char D_800F34F8[];
extern char D_8010BD00[];
extern char D_80120D08[];
extern char D_801ED800[];

extern volatile s32 D_800B0E24;
extern volatile s32 D_800B0E28;
extern volatile s32 D_800B0E2C;
extern volatile s32 D_800B0E30;
extern volatile s32 D_800B0E34;
extern volatile s32 g_OtBufferTable;
extern volatile s32 g_RenderOtBufferBaseAlt;
extern volatile s32 D_800B0E40;
extern volatile s32 D_800B0E44;
extern volatile s32 D_800B0E48;
extern volatile s32 g_RenderScratchBufferBase;
extern volatile s32 g_RenderFrontBufferBase;
extern volatile s32 g_RenderBackBufferBase;
extern volatile s32 g_RenderAnimBufferBase;
extern volatile s32 D_800B0E5C;
extern volatile s32 D_800B0E60;
extern volatile s32 g_LoadedSceneAssetBlock;
extern volatile s32 D_800B0E68;
extern volatile s32 g_SceneLoadScratchBuffer;

void Boot_InitMemoryLayout(void) {
    register s32 ptr asm("$2");
    s32 ptr2;
    register s32 const_8000 asm("$4");
    s32 const_48000;
    s32 tmp;

    const_48000 = 0x48000;

    ptr = (s32)D_800F34F8;
    ptr2 = ptr + 0x1800;
    D_800B0E24 = ptr;
    ptr += 0x6000;
    D_800B0E28 = ptr2;
    D_800B0E2C = ptr;
    ptr += 0xE000;
    D_800B0E30 = ptr;

    ptr = (s32)D_8010BD00;
    ptr2 = (s32)D_80120D08;
    D_800B0E40 = ptr;
    ptr = ptr2 + 0x1C98;
    D_800B0E34 = ptr2;
    ptr2 += 0x5C98;
    const_8000 = 0x8000;
    g_OtBufferTable = ptr;
    ptr += const_8000;
    g_RenderOtBufferBaseAlt = ptr2;
    ptr2 = ptr + 0x2400;
    D_800B0E44 = ptr;
    ptr += 0x4800;
    g_RenderScratchBufferBase = ptr;
    ptr += const_48000;
    D_800B0E48 = ptr2;
    ptr2 = ptr + 0x4000;
    g_RenderFrontBufferBase = ptr;
    ptr += const_8000;
    g_RenderBackBufferBase = ptr2;
    ptr2 = ptr + 0x3800;
    D_800B0E5C = ptr2;
    tmp = g_StrFileDirBuffer;
    g_RenderAnimBufferBase = ptr;
    ptr += 0x7000;
    D_800B0E60 = ptr;
    g_SceneLoadScratchBuffer = (s32)D_801ED800;
    g_LoadedSceneAssetBlock = tmp - 8;
    D_800B0E68 = tmp;
}
