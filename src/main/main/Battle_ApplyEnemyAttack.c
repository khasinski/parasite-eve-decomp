#include "common.h"
#include "pe1/battle.h"
#include "pe1/inventory.h"
#define NULL ((void *)0)

#define COMBATANT_ATTRIBUTES(base) \
    ((Combatant *)(base))->attributes
#define ATTRIBUTE_EFFECT_FLAGS(base) \
    ((BattleAttributes *)(base))->effectFlags

int Inv_FindItemById(int arg0);

#define D278_0 (*(u8 **)&D278_o16)
#define D278_1 (*(u8 **)&D278_o16)
#define D278_2 (*(u8 **)&D278_o17)
#define D278_3 ((u8 *)g_ActiveActor)
#define D278_4 (*(u8 **)&D278_o16)
#define D278_5 (*(u8 **)&D278_o17)
#define D278_6 ((u8 *)g_ActiveActor)
#define D278_7 (*(u8 **)&D278_o16)
#define D278_8 (*(u8 **)&D278_o17)
#define D278_9 ((u8 *)g_ActiveActor)
#define D278_10 (*(u8 **)&D278_o16)
#define D278_11 (*(u8 **)&D278_o17)
#define D278_12 ((u8 *)g_ActiveActor)
#define D278_13 (*(u8 **)&D278_o16)
#define D278_14 (*(u8 **)&D278_o17)
extern Combatant *g_ActiveActor;
#define D278_15 ((u8 *)g_ActiveActor)
extern struct { char _[16]; } D278_o16 __asm__("g_ActiveActor");
#define D278_16 (*(u8 **)&D278_o16)
extern struct { char _[16]; } D278_o17 __asm__("g_ActiveActor");
#define D278_17 (*(u8 **)&D278_o17)

extern struct { char _[16]; } D228_o __asm__("g_ActorEffectFlag100Timer");
#define g_ActorEffectFlag100Timer (*(s16 *)&D228_o)
extern struct { char _[16]; } D1CE_o __asm__("g_BattleSaveOverlayActive");
#define g_BattleSaveOverlayActive (*(s8 *)&D1CE_o)
extern struct { char _[16]; } D2E8_oa __asm__("g_FieldMoveLock");
extern struct { char _[16]; } D2E8_ob __asm__("g_FieldMoveLock");
#define D2E8A (*(s32 *)&D2E8_oa)
#define D2E8B (*(s32 *)&D2E8_ob)

void Battle_ApplyEnemyAttack(EnemyCombatant *ent) {
    s32 *ps = (s32 *)(D278_0 + 0x4C);

    switch (ent->effect->effectType) {
    case 1:
        if ((*ps & 3) == 1) {
            break;
        }
        if (ATTRIBUTE_EFFECT_FLAGS(COMBATANT_ATTRIBUTES(D278_1)) & 1) {
            if ((rand() % 100) < 0x46) {
                break;
            }
        }
        {
            s32 f = *ps;
            if ((f & 3) == 3) {
                *ps = f & ~3;
                break;
            }
            {
                Combatant *pD = (Combatant *)D278_2;
                u8 lvl;
                *ps = (f & ~3) | 1;
                pD->statusTimer40 = 0x2328;
                lvl = ent->effectLevel;
                pD->subActionCounter = 0;
                pD->subActionStep = lvl;
            }
        }
        {
            Combatant *pD = (Combatant *)D278_3;
            pD->subActionPeriod = ent->effectDuration;
        }
        break;
    case 2:
        {
            Combatant *pD;
            register s32 f asm("$2");
            pD = (Combatant *)D278_4;
            f = pD->stateFlags;
            g_ActorEffectFlag100Timer = 0;
            pD->stateFlags = f & ~0x100;
            f = ((volatile Combatant *)pD)->stateFlags;
            f &= ~0x200;
            f &= ~0x400;
            f &= ~0x800;
            pD->stateFlags = f;
        }
        break;
    case 3:
        if ((*ps & 0xC) == 4) {
            break;
        }
        if (ATTRIBUTE_EFFECT_FLAGS(COMBATANT_ATTRIBUTES(D278_5)) & 2) {
            if ((rand() % 100) < 0x3C) {
                break;
            }
        }
        {
            s32 f = *ps;
            s32 r;
            s32 a3v;
            s32 d;
            Combatant *pD;
            if ((f & 0xC) == 0xC) {
                *ps = f & ~0xC;
                break;
            }
            d = D2E8A;
            *ps = (f & ~0xC) | 4;
            D2E8B = d | 0x10;
            r = rand();
            pD = (Combatant *)D278_6;
            a3v = r;
            if (r < 0) {
                a3v = r + 3;
            }
            pD->statusTimer42 = 0x2328;
            pD->stateFlags = (pD->stateFlags & ~0x60000) | (((r - ((a3v >> 2) << 2)) & 3) << 17);
        }
        break;
    case 4:
        if ((*ps & 0x30) == 0x10) {
            break;
        }
        if (ATTRIBUTE_EFFECT_FLAGS(COMBATANT_ATTRIBUTES(D278_7)) & 4) {
            if ((rand() % 100) < 0x3C) {
                break;
            }
        }
        {
            s32 f = *ps;
            Combatant *pD;
            if ((f & 0x30) == 0x30) {
                *ps = f & ~0x30;
                break;
            }
            pD = (Combatant *)D278_8;
            *ps = (f & ~0x30) | 0x10;
            pD->statusTimer44 = 0x2328;
        }
        break;
    case 5:
        if ((*ps & 0xC0) == 0x80) {
            break;
        }
        if (ATTRIBUTE_EFFECT_FLAGS(COMBATANT_ATTRIBUTES(D278_9)) & 8) {
            if ((rand() % 100) < 0x32) {
                break;
            }
        }
        {
            s32 f = *ps;
            if ((f & 0xC0) == 0xC0) {
                *ps = f & ~0xC0;
                break;
            }
            if (f & 0x100) {
                *ps = f & ~0x100;
            }
            {
                s32 nf = (*ps & ~0xC0) | 0x80;
                Combatant *pD = (Combatant *)D278_10;
                *ps = nf;
                pD->statusTimer46 = 0x2328;
            }
        }
        break;
    case 6:
        {
            s32 m = *ps & 0xC0;
            if (m == 0x80) {
                break;
            }
            if (m == 0x40) {
                break;
            }
        }
        if (ATTRIBUTE_EFFECT_FLAGS(COMBATANT_ATTRIBUTES(D278_11)) & 8) {
            if ((rand() % 100) < 0x46) {
                break;
            }
        }
        {
            s32 f = *ps;
            if ((f & 0xC0) == 0xC0) {
                *ps = f & ~0xC0;
                break;
            }
            if (f & 0x100) {
                *ps = f & ~0x100;
            }
            {
                s32 nf = (*ps & ~0xC0) | 0x40;
                Combatant *pD = (Combatant *)D278_12;
                *ps = nf;
                pD->statusTimer46 = 0x2328;
            }
        }
        break;
    case 7:
        *ps = *ps | 0x1000;
        break;
    case 8:
        if (!(*ps & 0x200)) {
            Combatant *pD = (Combatant *)D278_13;
            s32 tmp = pD->curHP << 16;
            if ((tmp >> 16) >= 2) {
                *(s16 *)&pD->curHP = tmp >> 17;
            }
        }
        break;
    case 9:
        {
            Combatant *pD = (Combatant *)D278_14;
            s32 hp;
            s32 f;
            g_ActorEffectFlag100Timer = 0;
            hp = *(s16 *)&pD->curHP;
            pD->stateFlags = pD->stateFlags & ~0x100;
            f = ((volatile Combatant *)pD)->stateFlags;
            f &= ~0x200;
            f &= ~0x400;
            f &= ~0x800;
            pD->stateFlags = f;
            if (hp >= 2) {
                *(s16 *)&pD->curHP = 1;
            } else if (hp == 1) {
                *(s16 *)&pD->curHP = -1;
            }
        }
        break;
    case 10:
    case 11:
        if (ATTRIBUTE_EFFECT_FLAGS(COMBATANT_ATTRIBUTES(D278_15)) & 0x10) {
            s32 r = rand();
            if ((r % 100) >= 0x3C) {
                ent->lootItemId = Inv_PickRandomItem(r / 100);
                ent->effect->effectType = 0;
                Inv_FindItemById(ent->lootItemId);
                {
                    s32 it = ent->lootItemId;
                    g_BattleSaveOverlayActive = 1;
                    g_CurItemEffectData = Inv_GetItemEffectData(it, 0);
                }
            }
        }
        break;
    case 12:
    case 13:
        if (ATTRIBUTE_EFFECT_FLAGS(COMBATANT_ATTRIBUTES(D278_16)) & 0x10) {
            if ((rand() % 100) >= 0x3C) {
                Inv_RollRandomItemType(&ent->lootItemId,
                                       &ent->lootItemAux);
                ent->effect->effectType = 0;
                {
                    s32 it = ent->lootItemId;
                    g_BattleSaveOverlayActive = 1;
                    g_CurItemEffectData = Inv_GetItemEffectData(it, 0);
                }
            }
        }
        break;
    case 16:
        if (!(*ps & 0x200)) {
            Combatant *pD = (Combatant *)D278_17;
            s16 t = *(s16 *)&pD->curHP;
            if (t >= 2) {
                *(s16 *)&pD->curHP = (t * 3) / 4;
            }
        }
        break;
    }
}

#undef ATTRIBUTE_EFFECT_FLAGS
#undef COMBATANT_ATTRIBUTES
