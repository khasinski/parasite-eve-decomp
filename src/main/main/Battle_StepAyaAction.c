/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/battle.h"

typedef struct BattleTurnEntry {
    BattleEntity *entity;
    int field_04;
} BattleTurnEntry;

extern BattleEntity *D_8009D254;
extern Combatant *D_8009D278;
extern BattleEntity *D_8009D20C;
extern int D_8009D258;
extern int D_8009D2E8;
extern int D_8009D1A0;
extern unsigned char D_8009D1D4;
extern unsigned char D_8009D25C;
extern signed char D_8009CE48;
extern signed char D_8009CE54;
extern signed char D_8009CE55;
extern short D_8009CE4C;
extern int D_8009E054;
extern int D_8009E058;
extern int D_8009E05C;
extern unsigned short D_8009CE58;
extern unsigned short D_8009CE5C;
extern unsigned short D_800B0D88[];
extern unsigned char D_800B0D8A[];
extern unsigned short D_801F1F38[];
extern BattleTurnEntry D_800BE830[];
extern unsigned char D_800B0CEC[];

int Scene_InitEntityPlayer(int mode);
void Entity_SetActionMode(void *entity, int mode);
void Anim_SetInterpRate(void *anim, int rate);
int Scene_LoadRoomAssets(int room, void *entity);
void Pm_StopLowerHalf(void);
int Pm_Stop(int slot, void *entity, int mode);
int Battle_CalcAngleToTarget(void *from, void *to);
int rsin(int angle);
int rcos(int angle);
void Asset_Find08Alt(int id, int arg1, int x, int y, int z);

#define B8(ptr, off) (*(signed char *)((unsigned char *)(ptr) + (off)))
#define U8_AT(ptr, off) (*(unsigned char *)((unsigned char *)(ptr) + (off)))
#define S16_AT(ptr, off) (*(short *)((unsigned char *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(unsigned short *)((unsigned char *)(ptr) + (off)))
#define S32_AT(ptr, off) (*(int *)((unsigned char *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(unsigned int *)((unsigned char *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(void **)((unsigned char *)(ptr) + (off)))

static void Battle_AyaAdvanceState(void) {
    D_8009D25C++;
}

static int Battle_AyaTickTimer(void) {
    if (D_8009CE4C != 0) {
        D_8009CE4C--;
        return 1;
    }
    return 0;
}

static int Battle_AyaAnimAtFrame(BattleEntity *entity, int frame) {
    return U8_AT(entity, 0x0F) == frame;
}

static int Battle_AyaAnimAtEnd(BattleEntity *entity) {
    return U8_AT(entity, 0x0F) == U16_AT(entity, 0x1A);
}

static void Battle_AyaSetAnimFlags(int bits) {
    U16_AT(D_8009D254, 0x250) |= bits;
}

static void Battle_AyaSetRenderInterp(int rate) {
    Anim_SetInterpRate((unsigned char *)D_8009D254 + 0x1B4, rate);
}

static void Battle_AyaSavePose(BattleEntity *entity) {
    D_8009E054 = S32_AT(entity, 0x28);
    D_8009E058 = S32_AT(entity, 0x2C);
    D_8009E05C = S32_AT(entity, 0x30);
    D_8009CE58 = U16_AT(entity, 0x38);
    (&D_8009CE58)[1] = U16_AT(entity, 0x3A);
    D_8009CE5C = U16_AT(entity, 0x3C);
}

static void Battle_AyaRestorePose(BattleEntity *entity) {
    S32_AT(entity, 0x28) = D_8009E054;
    S32_AT(entity, 0x2C) = D_8009E058;
    S32_AT(entity, 0x30) = D_8009E05C;
    S32_AT(entity, 0x40) = D_8009E054;
    S32_AT(entity, 0x44) = D_8009E058;
    S32_AT(entity, 0x48) = D_8009E05C;
    U16_AT(entity, 0x38) = D_8009CE58;
    U16_AT(entity, 0x3A) = (&D_8009CE58)[1];
    U16_AT(entity, 0x3C) = D_8009CE5C;
}

static void Battle_AyaClearOtherVelocity(BattleEntity *self) {
    BattleEntity *entity;

    entity = D_8009D20C;
    while (entity != 0) {
        if (entity != self && entity->core != 0) {
            S32_AT(entity, 0x68) = 0;
            S32_AT(entity, 0x6C) = 0;
            S32_AT(entity, 0x70) = 0;
        }
        entity = entity->next;
    }
}

static BattleEntity *Battle_AyaCurrentTarget(void) {
    return D_800BE830[D_8009D1D4].entity;
}

static int Battle_AyaCurrentTargetIsRepeated(void) {
    return D_800BE830[D_8009D1D4].entity == D_800BE830[D_8009D1D4 + 1].entity;
}

static void Battle_AyaUpdateTargetRenderBits(void) {
    BattleEntity *target;
    Combatant *combatant;
    unsigned int flags;
    int weapon_class;

    target = Battle_AyaCurrentTarget();
    if (target == 0 || target->core == 0) {
        return;
    }

    flags = U32_AT(target->core, 0);
    flags &= ~0x6000;
    flags |= 0x2000;

    combatant = D_8009D278;
    weapon_class = 0;
    if (combatant != 0 && combatant->action != 0) {
        weapon_class = (combatant->action->attackWord >> 20) & 3;
    }
    flags &= 0xFFF3FFFF;
    flags |= weapon_class << 18;

    flags &= 0xFFFC7FFF;
    flags |= (D_8009CE55 & 7) << 15;
    U32_AT(target->core, 0) = flags;
}

static int Battle_AyaLoadShotEffect(void) {
    int effect;

    effect = 0x6E;
    if (D_8009CE48 == 2) {
        effect = 0x6F;
    } else if (D_8009CE48 == 5) {
        effect = 0x71;
    } else if (D_8009CE48 == 6) {
        effect = 0x70;
    }

    Scene_LoadRoomAssets(effect, Battle_AyaCurrentTarget());
    D_8009CE48++;

    if (D_8009CE48 >= 7) {
        Battle_AyaAdvanceState();
        return 0;
    }

    if (Battle_AyaCurrentTargetIsRepeated()) {
        D_8009D25C = 8;
        D_8009D1D4++;
    } else {
        Battle_AyaAdvanceState();
    }
    return 0;
}

static void Battle_AyaSetupTargetStep(void) {
    BattleEntity *entity;
    BattleEntity *target;
    Combatant *combatant;
    BattleAction *action;
    int range;
    int angle;
    int x;
    int z;

    entity = D_8009D254;
    target = Battle_AyaCurrentTarget();
    if (target == 0) {
        return;
    }

    combatant = D_8009D278;
    action = combatant != 0 ? combatant->action : 0;
    range = S16_AT(target, 0x224);
    if (action != 0) {
        range = range + (S16_AT(action, 0) * ((action->attackWord >> 19) & 0x1F)) / 10;
    }
    range += S16_AT(entity, 0x224);

    angle = Battle_CalcAngleToTarget((unsigned char *)target + 0x1B4, &D_8009E054);
    S16_AT(entity, 0x3A) = angle;

    x = S16_AT(target, 0x268) << 16;
    x += (range * rsin(angle)) << 4;
    S32_AT(entity, 0x28) = x;

    z = S16_AT(target, 0x26C) << 16;
    z += (range * rcos(S16_AT(entity, 0x3A))) << 4;
    S32_AT(entity, 0x30) = z;

    S32_AT(entity, 0x40) = S32_AT(entity, 0x28);
    S32_AT(entity, 0x44) = S32_AT(entity, 0x2C);
    S32_AT(entity, 0x48) = S32_AT(entity, 0x30);

    Asset_Find08Alt(0x4B6, 0, S16_AT(entity, 0x2A), S16_AT(entity, 0x2E), S16_AT(entity, 0x32));
    D_8009CE4C = 0x1E;
    Battle_AyaAdvanceState();
}

int Battle_StepAyaAction(void) {
    BattleEntity *entity;
    int result;

    result = 0;
    entity = D_8009D254;

    switch (D_8009D25C) {
    case 0:
        Scene_InitEntityPlayer(1);
        Battle_AyaSavePose(entity);
        D_8009CE48 = 0;
        D_8009D2E8 &= ~4;
        U32_AT(entity, 0x98) |= 0x80;
        Battle_AyaSetRenderInterp(0x1E);
        U16_AT(entity, 0x250) |= 2;
        Anim_SetInterpRate(D_800B0CEC, 0x1E);
        D_800B0D88[0] |= 2;
        Pm_StopLowerHalf();
        D_8009D258 = Scene_LoadRoomAssets(0x6B, entity);
        Battle_AyaAdvanceState();
        break;

    case 1:
        Scene_InitEntityPlayer(1);
        if (U8_AT(entity, 0x252) == 0 && D_800B0D8A[0] == 0) {
            Battle_AyaAdvanceState();
        }
        break;

    case 2:
        if (Scene_InitEntityPlayer(1) == 0) {
            Entity_SetActionMode(entity, 5);
            U8_AT(entity, 0x252) = 1;
            U32_AT(entity, 0x98) |= 0x100;
            Battle_AyaSetRenderInterp(0x1E);
            Battle_AyaSetAnimFlags(4);
            Scene_LoadRoomAssets(0x6C, entity);
            D_8009CE4C = 0x1E;
            Battle_AyaAdvanceState();
        }
        break;

    case 3:
        if (!Battle_AyaTickTimer()) {
            U32_AT(entity, 0x98) &= ~0x100;
            Battle_AyaAdvanceState();
        }
        if (Battle_AyaAnimAtFrame(entity, U16_AT(entity, 0x16))) {
            Battle_AyaClearOtherVelocity(entity);
            Pm_Stop(D_8009D258, entity, 0);
            Scene_LoadRoomAssets(0x6D, entity);
        }
        break;

    case 4:
        Entity_SetActionMode(entity, 6);
        Battle_AyaSetRenderInterp(0xF);
        Battle_AyaSetAnimFlags(2);
        Battle_AyaAdvanceState();
        break;

    case 5:
        if (U8_AT(entity, 0x252) == 0) {
            Battle_AyaSetupTargetStep();
        }
        break;

    case 6:
        if (Battle_AyaAnimAtEnd(entity)) {
            Entity_SetActionMode(entity, 7);
            S32_AT(entity, 0x14) = U8_AT(entity, 0x0F) << 15;
        }
        if (!Battle_AyaTickTimer()) {
            U8_AT(entity, 0x252) = 1;
            Battle_AyaSetRenderInterp(0xF);
            Battle_AyaSetAnimFlags(4);
            Battle_AyaAdvanceState();
        }
        break;

    case 7:
        if (Battle_AyaAnimAtEnd(entity)) {
            Battle_AyaSetAnimFlags(0x20);
            Entity_SetActionMode(entity, ((int)D_8009CE48 * 2 + 8) & ~1);
            Battle_AyaAdvanceState();
        }
        break;

    case 8:
        if (Battle_AyaAnimAtEnd(entity)) {
            Entity_SetActionMode(entity, ((int)D_8009CE48 * 2 + 9) & 0xFFFF);
            D_8009CE55 = 2;
            D_8009CE54 = 1;
            Battle_AyaUpdateTargetRenderBits();
            Battle_AyaLoadShotEffect();
        }
        break;

    case 9:
        if (Battle_AyaAnimAtEnd(entity)) {
            Entity_SetActionMode(entity, 7);
            if (D_8009CE48 < 7) {
                D_8009D25C = 5;
                D_8009D1D4++;
            } else {
                Battle_AyaSetRenderInterp(0xF);
                Battle_AyaSetAnimFlags(2);
                Battle_AyaAdvanceState();
            }
        }
        break;

    case 10:
        if (U8_AT(entity, 0x252) == 0) {
            Battle_AyaRestorePose(entity);
            Asset_Find08Alt(0x4B6, 0, S16_AT(entity, 0x2A), S16_AT(entity, 0x2E), S16_AT(entity, 0x32));
            D_8009CE4C = 0x1E;
            Battle_AyaAdvanceState();
        }
        break;

    case 11:
        if (!Battle_AyaTickTimer()) {
            Entity_SetActionMode(entity, 7);
            U8_AT(entity, 0x252) = 1;
            Battle_AyaSetRenderInterp(0x1E);
            D_801F1F38[0] = 1;
            D_8009CE4C = 0xF;
            Battle_AyaSetAnimFlags(4);
            Battle_AyaAdvanceState();
        }
        break;

    case 12:
        if (!Battle_AyaTickTimer() && Battle_AyaAnimAtEnd(entity)) {
            Entity_SetActionMode(entity, 4);
            Battle_AyaAdvanceState();
        }
        break;

    case 13:
        if (Battle_AyaAnimAtFrame(entity, U16_AT(entity, 0x16))) {
            U32_AT(entity, 0x98) |= 0x100;
            Battle_AyaSetRenderInterp(0xF);
            Battle_AyaSetAnimFlags(2);
            Scene_LoadRoomAssets(0x72, entity);
            Battle_AyaAdvanceState();
        }
        break;

    case 14:
        if (U8_AT(entity, 0x252) == 0 && Scene_InitEntityPlayer(0) == 0) {
            Entity_SetActionMode(entity, U8_AT(D_8009D278, 0x12));
            D_8009D2E8 |= 4;
            U8_AT(entity, 0x252) = 1;
            U32_AT(entity, 0x98) &= ~0x180;
            Battle_AyaSetRenderInterp(0x1E);
            Battle_AyaSetAnimFlags(4);
            D_800B0D8A[0] = 1;
            Anim_SetInterpRate(D_800B0CEC, 0x1E);
            D_8009CE4C = 0xF;
            D_800B0D88[0] |= 4;
            Battle_AyaAdvanceState();
        }
        break;

    case 15:
    case 16:
        if (!Battle_AyaTickTimer()) {
            D_8009D1A0 &= ~0x100;
            D_8009D1D4++;
            result = 1;
        }
        break;

    default:
        break;
    }

    return result;
}
