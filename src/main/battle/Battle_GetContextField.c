#include "common.h"
#include "pe1/battle.h"

extern BattleEntity *g_PlayerEntity;

int Battle_GetContextField(int arg0) {
    Combatant *ctx;
    int ret;
    u32 word;

    arg0 = (u8)arg0;
    ctx = g_PlayerEntity->core;
    ret = -1000;

    switch (arg0) {
    case 0:
        ret = ctx->coreFlags;
        break;
    case 1:
        ret = ctx->field04.fieldId04;
        break;
    case 2:
        ret = (u16)ctx->fieldId06;
        break;
    case 3:
        ret = ctx->exp_or_acc;
        break;
    case 4:
        ret = (s16)ctx->curHP;
        break;
    case 6:
        ret = ctx->hpAlive;
        break;
    case 7:
        ret = ctx->actionMode12;
        break;
    case 10:
        ret = (s16)ctx->maxHP;
        break;
    case 11:
        ret = (u16)ctx->stat1E;
        break;
    case 12:
        ret = (u16)ctx->stat20;
        break;
    case 13:
        ret = ctx->maxAtk;
        break;
    case 14:
        ret = ctx->stat22;
        break;
    case 20:
        word = ctx->stateFlags;
        ret = word >> 9;
        ret &= 1;
        break;
    case 21:
        word = ctx->stateFlags;
        ret = word >> 6;
        ret &= 3;
        break;
    case 22:
        word = ctx->stateFlags;
        ret = word >> 29;
        ret &= 1;
        break;
    }

    return ret;
}
