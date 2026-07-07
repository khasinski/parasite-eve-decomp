/* CC1_FLAGS: -g3 -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 Akao_ForEachVoiceMasked();
M2C_UNK Akao_SetNotePitch();
M2C_UNK func_80089960();
M2C_UNK func_80089B28();
M2C_UNK func_80089CF0();
extern struct { char _[16]; } D_8009B8F4_o __asm__("g_AkaoDefaultVoiceProgram");
#define g_AkaoDefaultVoiceProgram (*(M2C_UNK *)&D_8009B8F4_o)
extern struct { char _[16]; } D_8009CDE8_o __asm__("g_AkaoVoiceKeyOnState");
#define g_AkaoVoiceKeyOnState (*(s32 *)&D_8009CDE8_o)
extern struct { char _[16]; } D_8009D2C4_o __asm__("g_AkaoVoiceUpdateFlags");
#define g_AkaoVoiceUpdateFlags (*(s32 *)&D_8009D2C4_o)
extern struct { char _[16]; } g_AkaoCurTrack_o __asm__("g_AkaoCurTrack");
extern struct { char _[16]; } g_AkaoCurTrack_b __asm__("g_AkaoCurTrack");
extern struct { char _[16]; } g_AkaoCurTrack_c __asm__("g_AkaoCurTrack");
extern struct { char _[16]; } g_AkaoCurTrack_d __asm__("g_AkaoCurTrack");
#define g_AkaoCurTrack (*(void **)&g_AkaoCurTrack_o)
extern struct { char _[16]; } D_8009D2DC_o __asm__("g_AkaoSeqPendingFlags");
#define g_AkaoSeqPendingFlags (*(s32 *)&D_8009D2DC_o)
extern struct { char _[16]; } D_800B8AC0_o __asm__("g_AkaoVoiceStateTable");
#define g_AkaoVoiceStateTable (*(M2C_UNK **)&D_800B8AC0_o)
extern struct { char _[16]; } D_800BA560_o __asm__("g_AkaoVoiceStateTable2");
#define g_AkaoVoiceStateTable2 (*(M2C_UNK *)&D_800BA560_o)
extern struct { char _[16]; } D_800BCD50_o __asm__("g_SpuActiveVoiceMask");
#define g_SpuActiveVoiceMask (*(s32 *)&D_800BCD50_o)
extern s32 D_800BCD5C_r[] __asm__("g_SpuPendingKeyOffMask");
extern s32 D_800BCD5C_w[] __asm__("g_SpuPendingKeyOffMask");

void Akao_StepSequencerVoice(void *arg0) {
    register void *arg0v asm("$22");
    u8 *walk;
    s32 mask;
    s32 bit;
    u8 *pvoice;
    s32 kFFF5;
    register u8 *base asm("$16");
    s32 kFFFa;
    s32 tret;
    s32 tnor;
    s32 tcd;
    s32 tand;
    s32 t2dc;
    s32 tw1;
    s32 tw2;
    register s32 kFFFb asm("$3");
    s32 km102;
    register s32 tv0 asm("$2");
    s32 tcde;
    s32 tlhu;
    s32 var_v0;
    void *p2c8a;
    s32 tld;
    void *p2c8b;
    void *p2c8c;
    void *pend;
    register s32 kffe asm("$4");

    arg0v = arg0;
    walk = (u8 *)arg0v;
    kFFFa = 0xFFFFFF;
    p2c8a = g_AkaoCurTrack;
    M2C_FIELD(p2c8a, void **, 0x2C) = arg0v;
    tld = M2C_FIELD(arg0v, s32 *, 0);
    mask = tld & kFFFa;
    tret = Akao_ForEachVoiceMasked(&g_AkaoVoiceStateTable2, M2C_FIELD(p2c8a, s32 *, 0x6C));
    tnor = ~tret;
    tcd = ~g_SpuActiveVoiceMask & kFFFa;
    tand = tnor & tcd;
    p2c8b = (*(void **)&g_AkaoCurTrack_b);
    t2dc = g_AkaoSeqPendingFlags;
    D_800BCD5C_w[0] = D_800BCD5C_r[0] | tand;
    M2C_FIELD(p2c8b, s32 *, 0x18) = 0;
    if (t2dc & 1) {
        M2C_FIELD(p2c8b, s32 *, 4) = 0;
        M2C_FIELD(p2c8b, s32 *, 0x1C) = (s32) (M2C_FIELD(p2c8b, s32 *, 0x1C) | mask);
    } else {
        M2C_FIELD(p2c8b, s32 *, 0x1C) = 0;
        M2C_FIELD(p2c8b, s32 *, 4) = (s32) (M2C_FIELD(p2c8b, s32 *, 4) | mask);
    }
    walk += 4;
    __asm__ __volatile__("");
    kFFFb = 0xFFFFFF;
    tw1 = *(s32 *)walk;
    walk += 4;
    __asm__ __volatile__("");
    bit = 1;
    pvoice = (u8 *)&D_800B8AC0_o;
    kFFF5 = 0xFFFFFF;
    base = pvoice + 0x116;
    km102 = -0x102;
    __asm__ __volatile__("");
    p2c8c = (*(void **)&g_AkaoCurTrack_c);
    M2C_FIELD(p2c8c, s32 *, 8) = tw1 & kFFFb;
    tw2 = *(s32 *)walk;
    walk += 8;
    M2C_FIELD(p2c8c, s32 *, 0xC) = tw2 & kFFFb;
    __asm__ __volatile__("");
    tv0 = M2C_FIELD(p2c8c, s32 *, 0) & km102;
    tcde = g_AkaoVoiceKeyOnState & 0x100;
    *(volatile s32 *)p2c8c = tv0;
    M2C_FIELD(p2c8c, s32 *, 0) = tv0 | tcde;
    do {
        if (mask & bit) {
            tlhu = *(u16 *)walk;
            walk += 2;
            *(u8 **)pvoice = walk + tlhu;
            M2C_FIELD(base, s32 *, -0x26) = 0x18;
            M2C_FIELD(base, s16 *, -0xC0) = 4;
            M2C_FIELD(base, s16 *, -0xBE) = 2;
            M2C_FIELD(base, s16 *, -0xAA) = 0x7F00;
            M2C_FIELD(base, s32 *, -0xD2) = 0x3FFF0000;
            M2C_FIELD(base, s16 *, -0x3E) = 0x4000;
            M2C_FIELD(base, s16 *, -0x36) = 0;
            M2C_FIELD(base, s16 *, -0x38) = 0;
            M2C_FIELD(base, s16 *, -0x94) = 0;
            M2C_FIELD(base, s32 *, -0xE2) = 0;
            M2C_FIELD(base, s16 *, -0x32) = 0;
            M2C_FIELD(base, s16 *, -0x9C) = 0;
            M2C_FIELD(base, s16 *, -0x44) = 0;
            M2C_FIELD(base, s16 *, -0x46) = 0;
            M2C_FIELD(base, u16 *, -0xA0) = 0x8000;
            M2C_FIELD(base, s16 *, -0x9E) = 0;
            M2C_FIELD(base, s16 *, -0x94) = 0;
            M2C_FIELD(base, s16 *, -0xA2) = 0;
            M2C_FIELD(base, s16 *, -0xA4) = 0;
            M2C_FIELD(base, void **, -0x102) = arg0v;
            M2C_FIELD(base, s16 *, -0x92) = 0;
            M2C_FIELD(base, s16 *, -0x2A) = 0;
            M2C_FIELD(base, s32 *, -0xDE) = 0;
            M2C_FIELD(base, s16 *, -0x48) = 0;
            M2C_FIELD(base, s16 *, -0x62) = 0;
            M2C_FIELD(base, s16 *, -0x70) = 0;
            M2C_FIELD(base, s16 *, -0x82) = 0;
            M2C_FIELD(base, s16 *, -0x60) = 0;
            M2C_FIELD(base, s16 *, -0x6E) = 0;
            M2C_FIELD(base, s16 *, -0x80) = 0;
            M2C_FIELD(base, s16 *, -0x5A) = 0;
            M2C_FIELD(base, s16 *, -0x5C) = 0;
            Akao_SetNotePitch(pvoice, 0);
            var_v0 = ~bit;
        } else {
            M2C_FIELD(base, s16 *, -0xC0) = 3;
            M2C_FIELD(base, s16 *, -0xBE) = 1;
            *(void **)pvoice = (void *)&D_8009B8F4_o;
            M2C_FIELD(base, s16 *, 0) = 5;
            M2C_FIELD(base, s32 *, -0x22) = (s32) (M2C_FIELD(base, s32 *, -0x22) | 0x4400);
            __asm__ __volatile__("");
            var_v0 = ~bit;
        }
        kFFF5 &= var_v0;
        mask &= var_v0;
        base += 0x11C;
        pvoice += 0x11C;
        bit *= 2;
    } while (kFFF5 != 0);
    __asm__("" : "=r"(kFFF5) : "0"(kFFF5));
    pend = (*(void **)&g_AkaoCurTrack_d);
    M2C_FIELD(pend, s32 *, 0x20) = 0xFFFF0000;
    M2C_FIELD(pend, s32 *, 0x28) = 1;
    M2C_FIELD(pend, s16 *, 0x52) = 0;
    M2C_FIELD(pend, s32 *, 0x40) = 0;
    M2C_FIELD(pend, s16 *, 0x58) = 0;
    M2C_FIELD(pend, s32 *, 0x44) = 0;
    g_AkaoVoiceUpdateFlags = 0;
    M2C_FIELD(pend, s16 *, 0x62) = 0;
    M2C_FIELD(pend, s16 *, 0x60) = 0;
    M2C_FIELD(pend, s16 *, 0x5E) = 0;
    M2C_FIELD(pend, s16 *, 0x64) = 0;
    M2C_FIELD(pend, s32 *, 0x34) = 0;
    M2C_FIELD(pend, s32 *, 0x38) = 0;
    M2C_FIELD(pend, s32 *, 0x3C) = 0;
    M2C_FIELD(pend, s16 *, 0x56) = 0;
    M2C_FIELD(pend, s32 *, 0x10) = 0;
    kffe = 0xFFFFFF;
    __asm__ __volatile__("");
    M2C_FIELD(pend, s32 *, 0x14) = kffe;
    M2C_FIELD(pend, s32 *, 0x30) = 0;
    func_80089960(kffe);
    func_80089B28();
    func_80089CF0();
}
