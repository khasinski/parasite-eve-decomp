#include "pe1/battle_runtime.h"
#include "pe1/field_movement.h"

void Aya_DeriveStats(int *first, int *second);
void Gpu_SetupSprites(void *actor, int unused, int kind);

void Battle_ApplyDamage(int damageKind) {
    int effect;
    s8 result = -1;
    Combatant *actor;
    int flags;

    if ((D_8009D1A0 & 2) == 0) {
        BattleEntity *localActor = D_8009D254;
        D_8009D278 = localActor->core;
        effect = damageKind < 13 ? 0x4B4 : 0x4B5;
        Asset_Find08Alt(effect, 1, localActor->posX.parts.integer,
                        localActor->posY.parts.integer,
                        localActor->posZ.parts.integer);
    } else {
        BattleEntity *localActor;
        effect = damageKind < 13 ? 0x4B4 : 0x4B5;
        localActor = D_8009D254;
        Asset_Find08Alt(effect, 0, localActor->posX.parts.integer,
                        localActor->posY.parts.integer,
                        localActor->posZ.parts.integer);
    }

    switch (damageKind) {
    case 6:
        D_8009D278->curHP += 0x2D;
        break;
    case 7:
        D_8009D278->curHP += 0x5A;
        break;
    case 8:
        D_8009D278->curHP += 0xB4;
        break;
    case 9:
        D_8009D278->curHP += 0x190;
        break;
    case 10:
        D_8009D278->curHP = D_8009D278->maxHP;
        break;
    case 11:
        D_8009D278->exp_or_acc += D_8009D278->maxAtk / 4;
        break;
    case 12:
        D_8009D278->exp_or_acc += D_8009D278->maxAtk / 2;
        break;
    case 13: {
        Combatant *target = D_8009D278;
        int field = target->stateFlags;
        if ((field & 3) == 0) {
            target->stateFlags = field | 3;
        } else if ((field & 3) != 3) {
            target->stateFlags = field & ~3;
        }
        result = 7;
        break;
    }
    case 14: {
        Combatant *target = D_8009D278;
        int field = target->stateFlags;
        if ((field & 0x30) == 0) {
            target->stateFlags = field | 0x30;
        } else if ((field & 0x30) != 0x30) {
            target->stateFlags = field & ~0x30;
        }
        result = 5;
        break;
    }
    case 15: {
        Combatant *target = D_8009D278;
        int field = target->stateFlags;
        if ((field & 0x0C) == 0) {
            target->stateFlags = field | 0x0C;
        } else if ((field & 0x0C) != 0x0C) {
            target->stateFlags = field & ~0x0C;
            D_8009D2E8 &= ~0x10;
        }
        result = 4;
        break;
    }
    case 16: {
        Combatant *target = D_8009D278;
        int field = target->stateFlags;
        if ((field & 0xC0) == 0) {
            target->stateFlags = field | 0xC0;
        } else if ((field & 0xC0) != 0xC0) {
            target->stateFlags = field & ~0xC0;
        }
        result = 6;
        break;
    }
    case 17:
        actor = D_8009D278;
        Aya_DeriveStats(&actor->atbStep, &actor->atbRate);
        actor = D_8009D278;
        flags = actor->stateFlags;
        if ((flags & 3) != 3) {
            actor->stateFlags = flags & ~3;
        }
        actor = D_8009D278;
        flags = actor->stateFlags;
        if ((flags & 0x0C) != 0x0C) {
            actor->stateFlags = flags & ~0x0C;
            D_8009D2E8 &= ~0x10;
        }
        actor = D_8009D278;
        flags = actor->stateFlags;
        if ((flags & 0x30) != 0x30) {
            actor->stateFlags = flags & ~0x30;
        }
        actor = D_8009D278;
        flags = actor->stateFlags;
        if ((flags & 0xC0) != 0xC0) {
            actor->stateFlags = flags & ~0xC0;
        }
        D_8009D278->stateFlags &= ~0x1000;
        break;
    }

    {
        Combatant *endActor = D_8009D278;
        if ((s16)endActor->maxHP < (s16)endActor->curHP) {
            endActor->curHP = (s16)endActor->maxHP;
        }
    }
    if (result != -1 && (D_8009D1A0 & 2)) {
        Gpu_SetupSprites(D_8009D254, 0, (u8)result);
    }
}
