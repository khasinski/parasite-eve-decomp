#include "common.h"

extern u8 *D_8009D278;
extern u16 D_800A76D8;
extern u16 D_800A76DA;
extern u16 D_800A76DC;
extern u16 D_800A76DE;
extern u32 D_800A76E0;
extern volatile u32 D_800A76E4;
extern volatile u32 D_800A76E8;
extern volatile u16 D_800A76EA;

void Battle_CopyPadStateToRecord(void) {
    volatile u8 *record = *(u8 **)(D_8009D278 + 0x68);
    register u32 word_c asm("$4");
    register u32 word_10 asm("$3");
    u32 middle_mask = 0x000FFC00;
    u32 middle_keep = ~0x000FFC00U;
    u32 high_keep = ~0x00300000U;
    u32 bit15_keep = ~0x00008000U;
    u32 bit16_keep = ~0x00010000U;
    u32 bit17_keep = ~0x00020000U;

    word_c = *(volatile u32 *)(record + 0x0C);
    word_10 = *(volatile u32 *)(record + 0x10);
    *(volatile u16 *)(record + 0x00) = D_800A76D8;
    *(volatile u16 *)(record + 0x02) = D_800A76DA;
    *(volatile u16 *)(record + 0x04) = D_800A76DC;
    *(volatile u16 *)(record + 0x06) = D_800A76DE;
    *(volatile u32 *)(record + 0x08) = D_800A76E0;

    word_c = (word_c & ~0x000003FFU) | (D_800A76E4 & 0x000003FFU);
    *(volatile u32 *)(record + 0x0C) = word_c;
    word_c = (word_c & middle_keep) | (D_800A76E4 & middle_mask);
    high_keep = word_c & high_keep;
    *(volatile u32 *)(record + 0x0C) = word_c;
    high_keep |= D_800A76E4 & 0x00300000U;
    *(volatile u32 *)(record + 0x0C) = high_keep;

    word_10 = (word_10 & ~0x0000000FU) | (D_800A76E8 & 0x0000000FU);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & ~0x00000030U) | (D_800A76E8 & 0x00000030U);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & ~0x000000C0U) | (D_800A76E8 & 0x000000C0U);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & ~0x00000100U) | (D_800A76E8 & 0x00000100U);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & ~0x00000200U) | (D_800A76E8 & 0x00000200U);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & ~0x00000400U) | (D_800A76E8 & 0x00000400U);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & ~0x00000800U) | (D_800A76E8 & 0x00000800U);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & ~0x00001000U) | (D_800A76E8 & 0x00001000U);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & ~0x00006000U) | (D_800A76E8 & 0x00006000U);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & bit15_keep) | (D_800A76E8 & 0x00008000U);
    *(volatile u32 *)(record + 0x10) = word_10;
    word_10 = (word_10 & bit16_keep) | ((D_800A76EA & 1) << 16);
    bit17_keep = word_10 & bit17_keep;
    *(volatile u32 *)(record + 0x10) = word_10;
    bit17_keep |= D_800A76E8 & 0x00020000U;
    *(u32 *)(record + 0x10) = bit17_keep;
}

#include "pe1/battle.h"
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
extern struct { char _[16]; } g_ActiveActor_o __asm__("g_ActiveActor");
#define g_ActiveActor (*(void **)&g_ActiveActor_o)
extern struct { char _[16]; } D_800A76D8_o __asm__("D_800A76D8");
#define D_800A76D8 (*(u32 *)&D_800A76D8_o)
#define D_800A76DC (*(s32 *)&D_800A76DC_o)
extern struct { char _[16]; } D_800A76DC_o __asm__("D_800A76DC");
extern struct { char _[16]; } D_800A76DC_s0 __asm__("D_800A76DC");
extern struct { char _[16]; } D_800A76DC_s1 __asm__("D_800A76DC");
extern struct { char _[16]; } D_800A76DC_s2 __asm__("D_800A76DC");
extern struct { char _[16]; } D_800A76DC_s3 __asm__("D_800A76DC");
extern struct { char _[16]; } D_800A76DC_s4 __asm__("D_800A76DC");
extern struct { char _[16]; } D_800A76DC_s5 __asm__("D_800A76DC");
extern struct { char _[16]; } D_800A76DC_s6 __asm__("D_800A76DC");
extern struct { char _[16]; } D_800A76DC_s7 __asm__("D_800A76DC");
extern struct { char _[16]; } D_800A76DC_s8 __asm__("D_800A76DC");
extern struct { char _[16]; } D_800A76DC_s9 __asm__("D_800A76DC");
extern struct { char _[16]; } D_800A76DC_s10 __asm__("D_800A76DC");
extern struct { char _[16]; } D_800A76DC_s11 __asm__("D_800A76DC");
extern struct { char _[16]; } D_800A76DC_s12 __asm__("D_800A76DC");
extern struct { char _[16]; } D_800A76DC_s13 __asm__("D_800A76DC");
extern struct { char _[16]; } D_800A76DE_o __asm__("g_PadStateMirrorWord3");
#define g_PadStateMirrorWord3 (*(u16 *)&D_800A76DE_o)
#define COMBATANT_FIELD(base, type, member) \
    (*(type)((char *)(base) + PE1_OFFSETOF(Combatant, member)))
#define ATTRIBUTE_FIELD(base, type, member) \
    (*(type)((char *)(base) + PE1_OFFSETOF(BattleAttributes, member)))

void Battle_SyncEnemyAttributes(void) {
    u32 *p76d8;
    register u32 tdc asm("$3");
    s32 kFF00000;
    s32 tfinal;
    register s32 tdc2 asm("$2");
    s32 k20000;
    register s32 f4 asm("$2");
    s32 kFFC00;
    s32 kFFF003FF;
    s32 kF00FFFFF;
    s32 k0FFFFFFF;
    s32 kFFFF7FFF;
    s32 kFFFEFFFF;
    register s32 kFFFDFFFF asm("$9");
    s32 km400;
    s32 temp_a0;
    s32 temp_a0_2;
    s32 temp_a2;
    s32 temp_v0;
    s32 temp_v0_10;
    s32 temp_v0_11;
    s32 temp_v0_12;
    s32 temp_v0_13;
    register s32 temp_v0_14 asm("$2");
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s32 temp_v0_6;
    s32 temp_v0_7;
    s32 temp_v0_8;
    s32 temp_v0_9;
    void *temp_a3;

    kFFC00 = 0xFFC00;
    kFFF003FF = 0xFFF003FF;
    kF00FFFFF = 0xF00FFFFF;
    k0FFFFFFF = 0x0FFFFFFF;
    kFFFF7FFF = 0xFFFF7FFF;
    kFFFEFFFF = 0xFFFEFFFF;
    kFFFDFFFF = 0xFFFDFFFF;
    p76d8 = (u32 *)&D_800A76D8_o;
    km400 = -0x400;
    temp_a3 = COMBATANT_FIELD(g_ActiveActor, void **, attributes);
    tdc = *p76d8;
    temp_a0 = (ATTRIBUTE_FIELD(temp_a3, s32 *, parameterWord) & km400) | (tdc & 0x3FF);
    f4 = ATTRIBUTE_FIELD(temp_a3, s32 *, effectFlags);
    ATTRIBUTE_FIELD(temp_a3, s32 *, parameterWord) = temp_a0;
    tdc = *p76d8;
    temp_a0_2 = (temp_a0 & kFFF003FF) | (tdc & kFFC00);
    ATTRIBUTE_FIELD(temp_a3, s32 *, parameterWord) = temp_a0_2;
    tdc = *p76d8;
    temp_a2 = temp_a0_2 & kF00FFFFF;
    kFF00000 = 0x0FF00000;
    temp_a2 = temp_a2 | (tdc & kFF00000);
    ATTRIBUTE_FIELD(temp_a3, s32 *, parameterWord) = temp_a2;
    tdc = *p76d8;
    ATTRIBUTE_FIELD(temp_a3, s32 *, parameterWord) = (s32)((temp_a2 & k0FFFFFFF) | ((tdc >> 0x1C) << 0x1C));
    tdc = (*(s32 *)&D_800A76DC_s0);
    temp_v0 = (f4 & ~1) | (tdc & 1);
    ATTRIBUTE_FIELD(temp_a3, s32 *, effectFlags) = temp_v0;
    tdc = (*(s32 *)&D_800A76DC_s1);
    temp_v0_2 = (temp_v0 & ~2) | (tdc & 2);
    ATTRIBUTE_FIELD(temp_a3, s32 *, effectFlags) = temp_v0_2;
    tdc = (*(s32 *)&D_800A76DC_s2);
    temp_v0_3 = (temp_v0_2 & ~4) | (tdc & 4);
    ATTRIBUTE_FIELD(temp_a3, s32 *, effectFlags) = temp_v0_3;
    tdc = (*(s32 *)&D_800A76DC_s3);
    temp_v0_4 = (temp_v0_3 & ~8) | (tdc & 8);
    ATTRIBUTE_FIELD(temp_a3, s32 *, effectFlags) = temp_v0_4;
    tdc = (*(s32 *)&D_800A76DC_s4);
    temp_v0_5 = (temp_v0_4 & ~0x10) | (tdc & 0x10);
    ATTRIBUTE_FIELD(temp_a3, s32 *, effectFlags) = temp_v0_5;
    tdc = (*(s32 *)&D_800A76DC_s5);
    temp_v0_6 = (temp_v0_5 & ~0x1E0) | (tdc & 0x1E0);
    ATTRIBUTE_FIELD(temp_a3, s32 *, effectFlags) = temp_v0_6;
    tdc = (*(s32 *)&D_800A76DC_s6);
    temp_v0_7 = (temp_v0_6 & ~0x200) | (tdc & 0x200);
    ATTRIBUTE_FIELD(temp_a3, s32 *, effectFlags) = temp_v0_7;
    tdc = (*(s32 *)&D_800A76DC_s7);
    temp_v0_8 = (temp_v0_7 & ~0x400) | (tdc & 0x400);
    ATTRIBUTE_FIELD(temp_a3, s32 *, effectFlags) = temp_v0_8;
    tdc = (*(s32 *)&D_800A76DC_s8);
    temp_v0_9 = (temp_v0_8 & ~0x800) | (tdc & 0x800);
    ATTRIBUTE_FIELD(temp_a3, s32 *, effectFlags) = temp_v0_9;
    tdc = (*(s32 *)&D_800A76DC_s9);
    temp_v0_10 = (temp_v0_9 & ~0x1000) | (tdc & 0x1000);
    ATTRIBUTE_FIELD(temp_a3, s32 *, effectFlags) = temp_v0_10;
    tdc = (*(s32 *)&D_800A76DC_s10);
    temp_v0_11 = (temp_v0_10 & ~0x2000) | (tdc & 0x2000);
    ATTRIBUTE_FIELD(temp_a3, s32 *, effectFlags) = temp_v0_11;
    tdc = (*(s32 *)&D_800A76DC_s11);
    temp_v0_12 = (temp_v0_11 & ~0x4000) | (tdc & 0x4000);
    ATTRIBUTE_FIELD(temp_a3, s32 *, effectFlags) = temp_v0_12;
    tdc = (*(s32 *)&D_800A76DC_s12);
    temp_v0_13 = temp_v0_12 & kFFFF7FFF;
    temp_v0_13 = temp_v0_13 | (tdc & 0x8000);
    ATTRIBUTE_FIELD(temp_a3, s32 *, effectFlags) = temp_v0_13;
    tdc = g_PadStateMirrorWord3;
    temp_v0_14 = temp_v0_13 & kFFFEFFFF;
    temp_v0_14 = temp_v0_14 | ((tdc & 1) << 0x10);
    tfinal = temp_v0_14 & kFFFDFFFF;
    ATTRIBUTE_FIELD(temp_a3, s32 *, effectFlags) = temp_v0_14;
    tdc2 = (*(s32 *)&D_800A76DC_s13);
    k20000 = 0x20000;
    ATTRIBUTE_FIELD(temp_a3, s32 *, effectFlags) = (s32)(tfinal | (tdc2 & k20000));
}

#undef ATTRIBUTE_FIELD
#undef COMBATANT_FIELD
