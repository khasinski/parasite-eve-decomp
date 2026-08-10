#include "common.h"
#include "pe1/battle.h"
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

s32 Battle_GetEnemyContextField(void *arg0, s32 arg1) {
    Combatant *ctx = *(Combatant **)arg0;
    s32 ret = -1000;
    register u32 word asm("$2");
    s32 flags;

    switch (((u8)arg1) - 0x29) {
    case 0:
        ret = (s8)ctx->field04.bytes.rank;
        break;
    case 2:
        ret = ctx->curHP;
        break;
    case 3:
        ret = *(s32 *)&ctx->hpAlive;
        if (ret < 0) {
            ret = 0;
        }
        break;
    case 7:
        word = ctx->coreFlags;
        word >>= 17;
        word &= 0x70;
        word = (u32)ctx + word;
        ret = *(u8 *)(word + 0x1C);
        break;
    case 19:
        ret = ctx->field88;
        if (ret < 0) {
            ret = 0;
        }
        break;
    case 20:
        ret = ctx->field8C;
        break;
    case 36:
        word = ctx->coreFlags;
        ret = word >> 13;
        ret &= 3;
        break;
    case 41:
        ret = *((u8 *)ctx + 3);
        ret &= 0x3F;
        break;
    case 89:
        flags = ctx->statusFlags2;
        if (flags & 0x1000000) {
            word = ctx->coreFlags & 0xC0000;
            if (word == 0xC0000) {
                ctx->statusFlags2 = flags & 0xFEFFFFFF;
                ret = 1;
            } else {
                ret = 0;
            }
        } else {
            ret = 0;
        }
        break;
    }
    return ret;
}
