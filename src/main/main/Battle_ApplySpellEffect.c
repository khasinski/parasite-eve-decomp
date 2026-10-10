#include "common.h"
#include "pe1/battle.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)

#define COMBATANT_ATTRIBUTES(base) \
    ((Combatant *)(base))->attributes
#define ATTRIBUTE_EFFECT_FLAGS(base) \
    ((BattleAttributes *)(base))->effectFlags

extern BattleActionSoundTable D_80010760;

void Entity_SetActionMode(void *, s32, s32, s32);
s32 Akao_SendPositionalCmdStereo(s32 arg0, s32 arg1, s16 arg2, s16 arg3, s16 arg4);
s32 rand(void);

extern struct { char _[16]; } D_8009D1A0_o __asm__("g_GameStateFlags");
#define g_GameStateFlags (*(s32 *)&D_8009D1A0_o)
extern void *g_BattleActiveEntity;
#define D1AC_1 (*(s32 *)&D1AC_o22)
#define D1AC_2 (*(s32 *)&D1AC_o22)
#define D1AC_3 (*(s32 *)&D1AC_o22)
#define D1AC_4 (*(s32 *)&D1AC_o22)
#define D1AC_5 (*(s32 *)&D1AC_o22)
#define D1AC_6 (*(s32 *)&D1AC_o22)
#define D1AC_7 (*(s32 *)&D1AC_o22)
#define D1AC_8 (*(s32 *)&D1AC_o22)
#define D1AC_9 (*(s32 *)&D1AC_o22)
#define D1AC_10 (*(s32 *)&D1AC_o22)
#define D1AC_11 (*(s32 *)&D1AC_o22)
#define D1AC_12 (*(s32 *)&D1AC_o22)
#define D1AC_13 (*(s32 *)&D1AC_o22)
#define D1AC_14 (*(s32 *)&D1AC_o22)
#define D1AC_15 (*(s32 *)&D1AC_o23)
#define D1AC_16 (*(s32 *)&D1AC_o24)
#define D1AC_17 (*(s32 *)&D1AC_o25)
extern s32 g_BattleResultFlags[];
#define D1AC_18 (g_BattleResultFlags[0])
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
#define D278_1 ((u8 *)g_ActiveActor[0])
#define D278_2 ((u8 *)g_ActiveActor[0])
#define D278_3 ((u8 *)g_ActiveActor[0])
#define D278_4 ((u8 *)g_ActiveActor[0])
#define D278_5 ((u8 *)g_ActiveActor[0])
#define D278_6 ((u8 *)g_ActiveActor[0])
#define D278_7 (*(u8 **)&D278_o54)
#define D278_8 (*(u8 **)&D278_o54)
#define D278_9 (*(u8 **)&D278_o53)
#define D278_10 (*(u8 **)&D278_o53)
#define D278_11 ((u8 *)g_ActiveActor[0])
#define D278_12 (*(u8 **)&D278_o54)
#define D278_13 (*(u8 **)&D278_o53)
#define D278_14 ((u8 *)g_ActiveActor[0])
#define D278_15 (*(u8 **)&D278_o54)
#define D278_16 (*(u8 **)&D278_o53)
#define D278_17 ((u8 *)g_ActiveActor[0])
#define D278_18 (*(u8 **)&D278_o54)
#define D278_19 (*(u8 **)&D278_o53)
#define D278_20 (*(u8 **)&D278_o54)
#define D278_21 (*(u8 **)&D278_o54)
#define D278_22 ((u8 *)g_ActiveActor[0])
#define D278_23 (*(u8 **)&D278_o53)
#define D278_24 (*(u8 **)&D278_o54)
#define D278_25 ((u8 *)g_ActiveActor[0])
#define D278_26 (*(u8 **)&D278_o53)
#define D278_27 (*(u8 **)&D278_o54)
#define D278_28 ((u8 *)g_ActiveActor[0])
#define D278_29 (*(u8 **)&D278_o53)
#define D278_30 (*(u8 **)&D278_o54)
#define D278_31 ((u8 *)g_ActiveActor[0])
#define D278_32 (*(u8 **)&D278_o53)
#define D278_33 (*(u8 **)&D278_o54)
#define D278_34 ((u8 *)g_ActiveActor[0])
#define D278_35 (*(u8 **)&D278_o53)
#define D278_36 (*(u8 **)&D278_o54)
#define D278_37 ((u8 *)g_ActiveActor[0])
#define D278_38 (*(u8 **)&D278_o53)
#define D278_39 (*(u8 **)&D278_o54)
#define D278_40 ((u8 *)g_ActiveActor[0])
#define D278_41 (*(u8 **)&D278_o53)
#define D278_42 (*(u8 **)&D278_o54)
#define D278_43 ((u8 *)g_ActiveActor[0])
#define D278_44 (*(u8 **)&D278_o53)
#define D278_45 (*(u8 **)&D278_o54)
#define D278_46 ((u8 *)g_ActiveActor[0])
#define D278_47 (*(u8 **)&D278_o53)
#define D278_48 (*(u8 **)&D278_o54)
#define D278_49 ((u8 *)g_ActiveActor[0])
#define D278_50 (*(u8 **)&D278_o53)
#define D278_51 (*(u8 **)&D278_o54)
extern Combatant *g_ActiveActor[];
#define D278_52 ((u8 *)g_ActiveActor[0])
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

void Battle_ApplySpellEffect(u32 idx, BattleEntity *ent) {
    BattleActionSoundTable soundTable;
    s32 dmg;
    register Combatant *pshared asm("$2");

    dmg = 0;
    if (!(g_GameStateFlags & 2)) {
        soundTable = D_80010760;
        Akao_SendPositionalCmdStereo(soundTable.soundId[idx], 1, *(s16 *)(D254A + 0x2A), *(s16 *)(D254A + 0x2E), *(s16 *)(D254A + 0x32));
        D278_1 = *(u8 **)D254B;
    }
    switch (idx) {
    case 0:
        dmg = 0x3C0000;
        ((Combatant *)D278_2)->curHP = ((Combatant *)D278_2)->curHP + 0x1E;
        break;
    case 1:
        dmg = 0x780000;
        ((Combatant *)D278_3)->curHP = ((Combatant *)D278_3)->curHP + 0x3C;
        break;
    case 2:
        dmg = 0x01F40000;
        ((Combatant *)D278_4)->curHP = ((Combatant *)D278_4)->curHP + 0x118;
        break;
    case 3:
        {
            Combatant *p = (Combatant *)D278_5;
            s32 f = p->stateFlags;
            if ((f & 3) != 3) {
                p->stateFlags = f & ~3;
            }
            dmg = 0x640000;
        }
        break;
    case 4:
        {
            Combatant *p = (Combatant *)D278_6;
            s32 f = p->stateFlags;
            register s32 c asm("$2");
            if ((f & 3) != 3) {
                p->stateFlags = f & ~3;
                p = (Combatant *)D278_7;
                f = p->stateFlags;
            }
            c = 0xC;
            if ((f & 0xC) != c) {
                p->stateFlags = f & ~0xC;
                D2E8B = D2E8A & ~0x10;
            }
            p = (Combatant *)D278_8;
            f = p->stateFlags;
            if ((f & 0x30) != 0x30) {
                p->stateFlags = f & ~0x30;
                p = (Combatant *)D278_9;
                f = p->stateFlags;
            }
            c = 0xC0;
            if ((f & 0xC0) != c) {
                p->stateFlags = f & ~0xC0;
            }
        }
        pshared = (Combatant *)D278_10;
        dmg = 0x02580000;
        goto clear1000;
    case 5:
        {
            Combatant *p = (Combatant *)D278_11;
            p->stateFlags = p->stateFlags | 0x200;
            p->exp_or_acc = p->exp_or_acc - (p->maxAtk / 3);
        }
        break;
    case 7:
        {
            Combatant *e;
            s32 v;
            s32 m7;
            register s32 m5 asm("$5");
            s32 m8;
            s32 m9;
            m7 = 0xFFFF7FFF;
            m5 = 0xFFFEFFFF;
            m8 = 0xFFFDFFFF;
            m9 = 0xFFFBFFFF;
            e = (Combatant *)ent->core;
            v = D1AC_14;
            { s32 b = (*(u32 *)&e->statusFlags2 & 3) == 2; v &= ~0x1000; v |= b << 0xC; }
            D1AC_15 = v;
            { s32 b = (*(u32 *)&e->statusFlags2 & 0xC) == 8; v &= ~0x2000; v |= b << 0xD; }
            D1AC_16 = v;
            { s32 b = (*(u32 *)&e->statusFlags2 & 0x30) == 0x20; v &= ~0x4000; v |= b << 0xE; }
            D1AC_17 = v;
            { s32 b = (*(u32 *)&e->statusFlags2 & 0xC000) == 0x8000; v &= m7; v |= b << 0xF; }
            D1AC_18 = v;
            m5 = v & m5;
            { s32 b = (*(u32 *)&e->statusFlags2 & 0x30000) == 0x20000; m5 |= b << 0x10; }
            D1AC_19 = m5;
            { s32 b = (*(u32 *)&e->statusFlags2 & 0x3000) == 0x2000; m5 &= m8; m5 |= b << 0x11; }
            D1AC_20 = m5;
            { s32 b = (*(u32 *)&e->statusFlags2 & 0xC00) == 0x800; m5 &= m9; m5 |= b << 0x12; }
            D1AC_21 = m5;
            {
                register s32 sv asm("$2");
                if (((*(u32 *)&(*(Combatant **)&ent->core)->statusFlags2 >> 6) & 3) == 2) {
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
            s32 m = (*(u32 *)&((Combatant *)ent->core)->statusFlags2 >> 8) & 3;
            if (m != 0) {
                if (m == 2) {
                    goto poison8;
                }
            } else if (!(rand() & 1)) {
poison8:
                {
                    Combatant *e = (Combatant *)ent->core;
                    *(s32 *)&e->coreFlags = *(s32 *)&e->coreFlags | 1;
                }
            }
            dmg = 0x5A0000;
        }
        break;
    case 9:
        {
            Combatant *p = (Combatant *)D278_13;
            s32 f;
            s32 m;
            p->stateFlags = p->stateFlags | 0x100;
            f = ((volatile Combatant *)p)->stateFlags;
            g_ActorEffectFlag100Timer = 0x1C2;
            m = f & 0xC0;
            if (m == 0x40 || m == 0x80) {
                p->stateFlags = f & ~0xC0;
            }
            dmg = 0xC80000;
        }
        break;
    case 10:
        {
            s32 m = (*(u32 *)&((Combatant *)ent->core)->statusFlags2 >> 0xA) & 3;
            if (m != 0) {
                if (m == 2) {
                    goto conf10;
                }
            } else if (!(rand() & 1)) {
conf10:
                {
                    s32 r = rand();
                    Combatant *e = (Combatant *)ent->core;
                    s32 nv = *(s32 *)&e->coreFlags & ~0xE;
                    nv |= (((r % 3) + 3) & 7) * 2;
                    *(s32 *)&e->coreFlags = nv;
                    Entity_SetActionMode(ent, 4, r / 3, nv);
                    ent->entityFlags = ent->entityFlags | 0x1000;
                }
            }
            dmg = 0x960000;
        }
        break;
    case 11:
        dmg = 0x01900000;
        ((Combatant *)D278_15)->stateFlags = ((Combatant *)D278_15)->stateFlags | 0x400;
        break;
    case 12:
        dmg = 0x03E80000;
        ((Combatant *)D278_16)->stateFlags = ((Combatant *)D278_16)->stateFlags | 0x800;
        break;
    case 18:
        {
            Combatant *q = (Combatant *)D278_17;
            s32 g = q->stateFlags;
            Combatant *p;
            s32 f;
            register s32 c18 asm("$2");
            q->curHP = q->maxHP;
            if ((g & 3) != 3) {
                q->stateFlags = g & ~3;
            }
            p = (Combatant *)D278_19;
            f = p->stateFlags;
            if ((f & 0xC) != 0xC) {
                p->stateFlags = f & ~0xC;
                D2E8B = D2E8A & ~0x10;
                __asm__ __volatile__("");
                p = (Combatant *)D278_20;
                f = p->stateFlags;
            }
            c18 = 0x30;
            if ((f & 0x30) != c18) {
                p->stateFlags = f & ~0x30;
            }
            p = (Combatant *)D278_21;
            f = p->stateFlags;
            if ((f & 0xC0) != 0xC0) {
                p->stateFlags = f & ~0xC0;
            }
        }
        pshared = (Combatant *)D278_55;
        dmg = 0x05140000;
clear1000:
        pshared->stateFlags = pshared->stateFlags & ~0x1000;
        break;
    case 19:
        ((Combatant *)D278_23)->stateFlags = (((Combatant *)D278_23)->stateFlags | 0x80000) & 0xFFDFFFFF;
        break;
    }
    {
        Combatant *p = (Combatant *)D278_24;
        s16 t = (s16)p->maxHP;
        if (t < (s16)p->curHP) {
            p->curHP = t;
            p = (Combatant *)D278_26;
        }
        if (!(ATTRIBUTE_EFFECT_FLAGS(COMBATANT_ATTRIBUTES(p)) & 0x200)) {
            p->exp_or_acc = p->exp_or_acc - dmg;
            return;
        }
        p->exp_or_acc = p->exp_or_acc - ((dmg * 2) / 3);
    }
}

#undef ATTRIBUTE_EFFECT_FLAGS
#undef COMBATANT_ATTRIBUTES
