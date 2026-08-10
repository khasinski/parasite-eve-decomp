#include "common.h"
#include "pe1/battle.h"

extern BattleEntity *g_PlayerEntity;
extern s16 g_RoomRenderYOverride __asm__("D_800942EC");

void Battle_SetContextField(int arg0, int arg1) {
    Combatant *ctx;

    arg0 = (u8)arg0;
    ctx = g_PlayerEntity->core;

    switch (arg0) {
    case 0:
        ctx->coreFlags = arg1;
        break;
    case 1:
        ctx->field04.fieldId04 = arg1;
        break;
    case 2:
        ctx->fieldId06 = arg1;
        break;
    case 3:
        ctx->exp_or_acc = arg1;
        break;
    case 4:
        ctx->curHP = arg1;
        break;
    case 5:
        ctx->hpMirror = arg1;
        break;
    case 6:
        ctx->hpAlive = arg1;
        break;
    case 10:
        ctx->maxHP = arg1;
        break;
    case 11:
        ctx->stat1E = arg1;
        break;
    case 12:
        ctx->stat20 = arg1;
        break;
    case 14:
        ctx->stat22 = arg1;
        break;
    case 18:
        ctx->stat26 = arg1;
        break;
    case 30:
        ctx->panelA_val = arg1;
        break;
    case 31:
        ctx->panelA_timer = arg1;
        break;
    case 32:
        ctx->panelA_scale = arg1;
        break;
    case 33:
        ctx->panelB_val = arg1;
        break;
    case 34:
        ctx->panelB_timer = arg1;
        break;
    case 255:
        g_RoomRenderYOverride = arg1;
        break;
    }
}
