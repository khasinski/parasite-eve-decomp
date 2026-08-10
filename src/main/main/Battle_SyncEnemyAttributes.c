#include "common.h"
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
    ATTRIBUTE_FIELD(temp_a3, s32 *, parameterWord) = (s32) ((temp_a2 & k0FFFFFFF) | ((tdc >> 0x1C) << 0x1C));
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
    ATTRIBUTE_FIELD(temp_a3, s32 *, effectFlags) = (s32) (tfinal | (tdc2 & k20000));
}

#undef ATTRIBUTE_FIELD
#undef COMBATANT_FIELD
