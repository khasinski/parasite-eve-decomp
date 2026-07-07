/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)

typedef struct { char b[0x28]; } __attribute__((aligned(1), packed)) Tbl40;

void Entity_SetActionMode(void *, s32, s32, s32);
void Akao_SendPositionalCmdStereo(u16, s32, s16, s16, s32);
s32 rand(void);

extern Tbl40 D_80010760;
extern struct { char _[16]; } D_8009D1A0_o __asm__("g_GameStateFlags");
#define g_GameStateFlags (*(s32 *)&D_8009D1A0_o)
extern void *g_BattleActiveEntity;
extern struct { char _[16]; } D1AC_o1 __asm__("g_BattleResultFlags");
#define D1AC_1 (*(s32 *)&D1AC_o1)
extern struct { char _[16]; } D1AC_o2 __asm__("g_BattleResultFlags");
#define D1AC_2 (*(s32 *)&D1AC_o2)
extern struct { char _[16]; } D1AC_o3 __asm__("g_BattleResultFlags");
#define D1AC_3 (*(s32 *)&D1AC_o3)
extern struct { char _[16]; } D1AC_o4 __asm__("g_BattleResultFlags");
#define D1AC_4 (*(s32 *)&D1AC_o4)
extern struct { char _[16]; } D1AC_o5 __asm__("g_BattleResultFlags");
#define D1AC_5 (*(s32 *)&D1AC_o5)
extern struct { char _[16]; } D1AC_o6 __asm__("g_BattleResultFlags");
#define D1AC_6 (*(s32 *)&D1AC_o6)
extern struct { char _[16]; } D1AC_o7 __asm__("g_BattleResultFlags");
#define D1AC_7 (*(s32 *)&D1AC_o7)
extern struct { char _[16]; } D1AC_o8 __asm__("g_BattleResultFlags");
#define D1AC_8 (*(s32 *)&D1AC_o8)
extern struct { char _[16]; } D1AC_o9 __asm__("g_BattleResultFlags");
#define D1AC_9 (*(s32 *)&D1AC_o9)
extern struct { char _[16]; } D1AC_o10 __asm__("g_BattleResultFlags");
#define D1AC_10 (*(s32 *)&D1AC_o10)
extern struct { char _[16]; } D1AC_o11 __asm__("g_BattleResultFlags");
#define D1AC_11 (*(s32 *)&D1AC_o11)
extern struct { char _[16]; } D1AC_o12 __asm__("g_BattleResultFlags");
#define D1AC_12 (*(s32 *)&D1AC_o12)
extern struct { char _[16]; } D1AC_o13 __asm__("g_BattleResultFlags");
#define D1AC_13 (*(s32 *)&D1AC_o13)
extern struct { char _[16]; } D1AC_o14 __asm__("g_BattleResultFlags");
#define D1AC_14 (*(s32 *)&D1AC_o14)
extern struct { char _[16]; } D1AC_o15 __asm__("g_BattleResultFlags");
#define D1AC_15 (*(s32 *)&D1AC_o15)
extern struct { char _[16]; } D1AC_o16 __asm__("g_BattleResultFlags");
#define D1AC_16 (*(s32 *)&D1AC_o16)
extern struct { char _[16]; } D1AC_o17 __asm__("g_BattleResultFlags");
#define D1AC_17 (*(s32 *)&D1AC_o17)
extern struct { char _[16]; } D1AC_o18 __asm__("g_BattleResultFlags");
#define D1AC_18 (*(s32 *)&D1AC_o18)
extern struct { char _[16]; } D1AC_o19 __asm__("g_BattleResultFlags");
#define D1AC_19 (*(s32 *)&D1AC_o19)
extern struct { char _[16]; } D1AC_o20 __asm__("g_BattleResultFlags");
#define D1AC_20 (*(s32 *)&D1AC_o20)
extern struct { char _[16]; } D1AC_o21 __asm__("g_BattleResultFlags");
#define D1AC_21 (*(s32 *)&D1AC_o21)
extern struct { char _[16]; } D1AC_o22 __asm__("g_BattleResultFlags");
#define D1AC_22 (*(s32 *)&D1AC_o22)
extern struct { char _[16]; } D1AC_o23 __asm__("g_BattleResultFlags");
#define D1AC_23 (*(s32 *)&D1AC_o23)
extern struct { char _[16]; } D1AC_o24 __asm__("g_BattleResultFlags");
#define D1AC_24 (*(s32 *)&D1AC_o24)
extern struct { char _[16]; } D1AC_o25 __asm__("g_BattleResultFlags");
#define D1AC_25 (*(s32 *)&D1AC_o25)
extern s16 g_ActorEffectFlag100Timer;
extern struct { char _[16]; } g_PlayerEntity_oa __asm__("g_PlayerEntity");
extern struct { char _[16]; } g_PlayerEntity_ob __asm__("g_PlayerEntity");
#define D254A (*(u8 **)&g_PlayerEntity_oa)
#define D254B (*(u8 **)&g_PlayerEntity_ob)
extern struct { char _[16]; } D278_o1 __asm__("g_ActiveActor");
#define D278_1 (*(u8 **)&D278_o1)
extern struct { char _[16]; } D278_o2 __asm__("g_ActiveActor");
#define D278_2 (*(u8 **)&D278_o2)
extern struct { char _[16]; } D278_o3 __asm__("g_ActiveActor");
#define D278_3 (*(u8 **)&D278_o3)
extern struct { char _[16]; } D278_o4 __asm__("g_ActiveActor");
#define D278_4 (*(u8 **)&D278_o4)
extern struct { char _[16]; } D278_o5 __asm__("g_ActiveActor");
#define D278_5 (*(u8 **)&D278_o5)
extern struct { char _[16]; } D278_o6 __asm__("g_ActiveActor");
#define D278_6 (*(u8 **)&D278_o6)
extern struct { char _[16]; } D278_o7 __asm__("g_ActiveActor");
#define D278_7 (*(u8 **)&D278_o7)
extern struct { char _[16]; } D278_o8 __asm__("g_ActiveActor");
#define D278_8 (*(u8 **)&D278_o8)
extern struct { char _[16]; } D278_o9 __asm__("g_ActiveActor");
#define D278_9 (*(u8 **)&D278_o9)
extern struct { char _[16]; } D278_o10 __asm__("g_ActiveActor");
#define D278_10 (*(u8 **)&D278_o10)
extern struct { char _[16]; } D278_o11 __asm__("g_ActiveActor");
#define D278_11 (*(u8 **)&D278_o11)
extern struct { char _[16]; } D278_o12 __asm__("g_ActiveActor");
#define D278_12 (*(u8 **)&D278_o12)
extern struct { char _[16]; } D278_o13 __asm__("g_ActiveActor");
#define D278_13 (*(u8 **)&D278_o13)
extern struct { char _[16]; } D278_o14 __asm__("g_ActiveActor");
#define D278_14 (*(u8 **)&D278_o14)
extern struct { char _[16]; } D278_o15 __asm__("g_ActiveActor");
#define D278_15 (*(u8 **)&D278_o15)
extern struct { char _[16]; } D278_o16 __asm__("g_ActiveActor");
#define D278_16 (*(u8 **)&D278_o16)
extern struct { char _[16]; } D278_o17 __asm__("g_ActiveActor");
#define D278_17 (*(u8 **)&D278_o17)
extern struct { char _[16]; } D278_o18 __asm__("g_ActiveActor");
#define D278_18 (*(u8 **)&D278_o18)
extern struct { char _[16]; } D278_o19 __asm__("g_ActiveActor");
#define D278_19 (*(u8 **)&D278_o19)
extern struct { char _[16]; } D278_o20 __asm__("g_ActiveActor");
#define D278_20 (*(u8 **)&D278_o20)
extern struct { char _[16]; } D278_o21 __asm__("g_ActiveActor");
#define D278_21 (*(u8 **)&D278_o21)
extern struct { char _[16]; } D278_o22 __asm__("g_ActiveActor");
#define D278_22 (*(u8 **)&D278_o22)
extern struct { char _[16]; } D278_o23 __asm__("g_ActiveActor");
#define D278_23 (*(u8 **)&D278_o23)
extern struct { char _[16]; } D278_o24 __asm__("g_ActiveActor");
#define D278_24 (*(u8 **)&D278_o24)
extern struct { char _[16]; } D278_o25 __asm__("g_ActiveActor");
#define D278_25 (*(u8 **)&D278_o25)
extern struct { char _[16]; } D278_o26 __asm__("g_ActiveActor");
#define D278_26 (*(u8 **)&D278_o26)
extern struct { char _[16]; } D278_o27 __asm__("g_ActiveActor");
#define D278_27 (*(u8 **)&D278_o27)
extern struct { char _[16]; } D278_o28 __asm__("g_ActiveActor");
#define D278_28 (*(u8 **)&D278_o28)
extern struct { char _[16]; } D278_o29 __asm__("g_ActiveActor");
#define D278_29 (*(u8 **)&D278_o29)
extern struct { char _[16]; } D278_o30 __asm__("g_ActiveActor");
#define D278_30 (*(u8 **)&D278_o30)
extern struct { char _[16]; } D278_o31 __asm__("g_ActiveActor");
#define D278_31 (*(u8 **)&D278_o31)
extern struct { char _[16]; } D278_o32 __asm__("g_ActiveActor");
#define D278_32 (*(u8 **)&D278_o32)
extern struct { char _[16]; } D278_o33 __asm__("g_ActiveActor");
#define D278_33 (*(u8 **)&D278_o33)
extern struct { char _[16]; } D278_o34 __asm__("g_ActiveActor");
#define D278_34 (*(u8 **)&D278_o34)
extern struct { char _[16]; } D278_o35 __asm__("g_ActiveActor");
#define D278_35 (*(u8 **)&D278_o35)
extern struct { char _[16]; } D278_o36 __asm__("g_ActiveActor");
#define D278_36 (*(u8 **)&D278_o36)
extern struct { char _[16]; } D278_o37 __asm__("g_ActiveActor");
#define D278_37 (*(u8 **)&D278_o37)
extern struct { char _[16]; } D278_o38 __asm__("g_ActiveActor");
#define D278_38 (*(u8 **)&D278_o38)
extern struct { char _[16]; } D278_o39 __asm__("g_ActiveActor");
#define D278_39 (*(u8 **)&D278_o39)
extern struct { char _[16]; } D278_o40 __asm__("g_ActiveActor");
#define D278_40 (*(u8 **)&D278_o40)
extern struct { char _[16]; } D278_o41 __asm__("g_ActiveActor");
#define D278_41 (*(u8 **)&D278_o41)
extern struct { char _[16]; } D278_o42 __asm__("g_ActiveActor");
#define D278_42 (*(u8 **)&D278_o42)
extern struct { char _[16]; } D278_o43 __asm__("g_ActiveActor");
#define D278_43 (*(u8 **)&D278_o43)
extern struct { char _[16]; } D278_o44 __asm__("g_ActiveActor");
#define D278_44 (*(u8 **)&D278_o44)
extern struct { char _[16]; } D278_o45 __asm__("g_ActiveActor");
#define D278_45 (*(u8 **)&D278_o45)
extern struct { char _[16]; } D278_o46 __asm__("g_ActiveActor");
#define D278_46 (*(u8 **)&D278_o46)
extern struct { char _[16]; } D278_o47 __asm__("g_ActiveActor");
#define D278_47 (*(u8 **)&D278_o47)
extern struct { char _[16]; } D278_o48 __asm__("g_ActiveActor");
#define D278_48 (*(u8 **)&D278_o48)
extern struct { char _[16]; } D278_o49 __asm__("g_ActiveActor");
#define D278_49 (*(u8 **)&D278_o49)
extern struct { char _[16]; } D278_o50 __asm__("g_ActiveActor");
#define D278_50 (*(u8 **)&D278_o50)
extern struct { char _[16]; } D278_o51 __asm__("g_ActiveActor");
#define D278_51 (*(u8 **)&D278_o51)
extern struct { char _[16]; } D278_o52 __asm__("g_ActiveActor");
#define D278_52 (*(u8 **)&D278_o52)
extern struct { char _[16]; } D278_o53 __asm__("g_ActiveActor");
#define D278_53 (*(u8 **)&D278_o53)
extern struct { char _[16]; } D278_o54 __asm__("g_ActiveActor");
#define D278_54 (*(u8 **)&D278_o54)
extern struct { char _[16]; } D278_o55 __asm__("g_ActiveActor");
#define D278_55 (*(u8 **)&D278_o55)
extern struct { char _[16]; } g_FieldMoveLock_oa __asm__("g_FieldMoveLock");
extern struct { char _[16]; } g_FieldMoveLock_ob __asm__("g_FieldMoveLock");
#define D2E8A (*(s32 *)&g_FieldMoveLock_oa)
#define D2E8B (*(s32 *)&g_FieldMoveLock_ob)

void Battle_ApplySpellEffect(u32 idx, u8 *ent) {
    union {
        Tbl40 t;
        u16 w[20];
    } buf;
    s32 dmg;
    register u8 *pshared asm("$2");

    dmg = 0;
    if (!(g_GameStateFlags & 2)) {
        buf.t = D_80010760;
        Akao_SendPositionalCmdStereo(buf.w[idx], 1, *(s16 *)(D254A + 0x2A), *(s16 *)(D254A + 0x2E), *(s16 *)(D254A + 0x32));
        D278_1 = *(u8 **)D254B;
    }
    switch (idx) {
    case 0:
        dmg = 0x3C0000;
        *(u16 *)(D278_2 + 0xC) = *(u16 *)(D278_2 + 0xC) + 0x1E;
        break;
    case 1:
        dmg = 0x780000;
        *(u16 *)(D278_3 + 0xC) = *(u16 *)(D278_3 + 0xC) + 0x3C;
        break;
    case 2:
        dmg = 0x01F40000;
        *(u16 *)(D278_4 + 0xC) = *(u16 *)(D278_4 + 0xC) + 0x118;
        break;
    case 3:
        {
            u8 *p = D278_5;
            s32 f = *(s32 *)(p + 0x4C);
            if ((f & 3) != 3) {
                *(s32 *)(p + 0x4C) = f & ~3;
            }
            dmg = 0x640000;
        }
        break;
    case 4:
        {
            u8 *p = D278_6;
            s32 f = *(s32 *)(p + 0x4C);
            register s32 c asm("$2");
            if ((f & 3) != 3) {
                *(s32 *)(p + 0x4C) = f & ~3;
                p = D278_7;
                f = *(s32 *)(p + 0x4C);
            }
            c = 0xC;
            if ((f & 0xC) != c) {
                *(s32 *)(p + 0x4C) = f & ~0xC;
                D2E8B = D2E8A & ~0x10;
            }
            p = D278_8;
            f = *(s32 *)(p + 0x4C);
            if ((f & 0x30) != 0x30) {
                *(s32 *)(p + 0x4C) = f & ~0x30;
                p = D278_9;
                f = *(s32 *)(p + 0x4C);
            }
            c = 0xC0;
            if ((f & 0xC0) != c) {
                *(s32 *)(p + 0x4C) = f & ~0xC0;
            }
        }
        pshared = D278_10;
        dmg = 0x02580000;
        goto clear1000;
    case 5:
        {
            u8 *p = D278_11;
            *(s32 *)(p + 0x4C) = *(s32 *)(p + 0x4C) | 0x200;
            *(s32 *)(p + 8) = *(s32 *)(p + 8) - (*(s32 *)(p + 0x28) / 3);
        }
        break;
    case 7:
        {
            u8 *e;
            s32 v;
            s32 m7;
            register s32 m5 asm("$5");
            s32 m8;
            s32 m9;
            m7 = 0xFFFF7FFF;
            m5 = 0xFFFEFFFF;
            m8 = 0xFFFDFFFF;
            m9 = 0xFFFBFFFF;
            __asm__ __volatile__("" :: "r"(m7), "r"(m5), "r"(m8), "r"(m9));
            e = *(u8 **)ent;
            v = D1AC_14;
            { s32 b = (*(u32 *)(e + 0xCC) & 3) == 2; v &= ~0x1000; v |= b << 0xC; }
            D1AC_15 = v;
            { s32 b = (*(u32 *)(e + 0xCC) & 0xC) == 8; v &= ~0x2000; v |= b << 0xD; }
            D1AC_16 = v;
            { s32 b = (*(u32 *)(e + 0xCC) & 0x30) == 0x20; v &= ~0x4000; v |= b << 0xE; }
            D1AC_17 = v;
            { s32 b = (*(u32 *)(e + 0xCC) & 0xC000) == 0x8000; v &= m7; v |= b << 0xF; }
            D1AC_18 = v;
            m5 = v & m5;
            { s32 b = (*(u32 *)(e + 0xCC) & 0x30000) == 0x20000; m5 |= b << 0x10; }
            D1AC_19 = m5;
            { s32 b = (*(u32 *)(e + 0xCC) & 0x3000) == 0x2000; m5 &= m8; m5 |= b << 0x11; }
            D1AC_20 = m5;
            { s32 b = (*(u32 *)(e + 0xCC) & 0xC00) == 0x800; m5 &= m9; m5 |= b << 0x12; }
            D1AC_21 = m5;
            {
                register s32 sv asm("$2");
                if (((*(u32 *)(*(u8 **)ent + 0xCC) >> 6) & 3) == 2) {
                    sv = (m5 & ~0xC00) | 0x400;
                } else {
                    sv = m5 & ~0xC00;
                }
                D1AC_22 = sv;
            }
            *(s8 *)&D1AC_o23 = 0x4B;
            g_BattleActiveEntity = ent;
            D1AC_24 = (D1AC_25 & ~0x300) | 0x100;
            dmg = 0x320000;
        }
        break;
    case 8:
        {
            s32 m = (*(u32 *)(*(u8 **)ent + 0xCC) >> 8) & 3;
            if (m != 0) {
                if (m == 2) {
                    goto poison8;
                }
            } else if (!(rand() & 1)) {
poison8:
                {
                    u8 *e = *(u8 **)ent;
                    *(s32 *)e = *(s32 *)e | 1;
                }
            }
            dmg = 0x5A0000;
        }
        break;
    case 9:
        {
            u8 *p = D278_13;
            s32 f;
            s32 m;
            *(s32 *)(p + 0x4C) = *(s32 *)(p + 0x4C) | 0x100;
            f = *(volatile s32 *)(p + 0x4C);
            g_ActorEffectFlag100Timer = 0x1C2;
            m = f & 0xC0;
            if (m == 0x40 || m == 0x80) {
                *(s32 *)(p + 0x4C) = f & ~0xC0;
            }
            dmg = 0xC80000;
        }
        break;
    case 10:
        {
            s32 m = (*(u32 *)(*(u8 **)ent + 0xCC) >> 0xA) & 3;
            if (m != 0) {
                if (m == 2) {
                    goto conf10;
                }
            } else if (!(rand() & 1)) {
conf10:
                {
                    s32 r = rand();
                    u8 *e = *(u8 **)ent;
                    s32 nv = *(s32 *)e & ~0xE;
                    nv |= (((r % 3) + 3) & 7) * 2;
                    *(s32 *)e = nv;
                    Entity_SetActionMode(ent, 4, r / 3, nv);
                    *(s32 *)(ent + 0x98) = *(s32 *)(ent + 0x98) | 0x1000;
                }
            }
            dmg = 0x960000;
        }
        break;
    case 11:
        dmg = 0x01900000;
        *(s32 *)(D278_15 + 0x4C) = *(s32 *)(D278_15 + 0x4C) | 0x400;
        break;
    case 12:
        dmg = 0x03E80000;
        *(s32 *)(D278_16 + 0x4C) = *(s32 *)(D278_16 + 0x4C) | 0x800;
        break;
    case 18:
        {
            u8 *q = D278_17;
            s32 g = *(s32 *)(q + 0x4C);
            u8 *p;
            s32 f;
            register s32 c18 asm("$2");
            *(u16 *)(q + 0xC) = *(u16 *)(q + 0x1C);
            if ((g & 3) != 3) {
                *(s32 *)(q + 0x4C) = g & ~3;
            }
            p = D278_19;
            f = *(s32 *)(p + 0x4C);
            if ((f & 0xC) != 0xC) {
                *(s32 *)(p + 0x4C) = f & ~0xC;
                D2E8B = D2E8A & ~0x10;
                __asm__ __volatile__("");
                p = D278_20;
                f = *(s32 *)(p + 0x4C);
            }
            c18 = 0x30;
            if ((f & 0x30) != c18) {
                *(s32 *)(p + 0x4C) = f & ~0x30;
            }
            p = D278_21;
            f = *(s32 *)(p + 0x4C);
            if ((f & 0xC0) != 0xC0) {
                *(s32 *)(p + 0x4C) = f & ~0xC0;
            }
        }
        pshared = D278_55;
        dmg = 0x05140000;
clear1000:
        *(s32 *)(pshared + 0x4C) = *(s32 *)(pshared + 0x4C) & ~0x1000;
        break;
    case 19:
        *(s32 *)(D278_23 + 0x4C) = (*(s32 *)(D278_23 + 0x4C) | 0x80000) & 0xFFDFFFFF;
        break;
    }
    {
        u8 *p = D278_24;
        s16 t = *(s16 *)(p + 0x1C);
        if (t < *(s16 *)(p + 0xC)) {
            *(s16 *)(p + 0xC) = t;
            p = D278_26;
        }
        if (!(*(s32 *)(*(u8 **)(p + 0x6C) + 4) & 0x200)) {
            *(s32 *)(p + 8) = *(s32 *)(p + 8) - dmg;
            return;
        }
        *(s32 *)(p + 8) = *(s32 *)(p + 8) - ((dmg * 2) / 3);
    }
}
