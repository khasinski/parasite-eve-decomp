/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "pe1/battle_reset_stats.h"

void Battle_ResetEnemyStats(int mode) {
    register Combatant *actor asm("$5");
    Combatant *clear_actor;
    register s32 flags asm("$2");

    actor = D_8009D278;
    if ((s16)actor->maxHP < (s16)actor->curHP) {
        actor->curHP = actor->maxHP;
    }

    D_8009D278->actionMode12 = 4;
    actor = D_8009D278;
    actor->hpAlive = 0;
    actor->atbGauge = 0;
    actor->hpMirror = actor->curHP;
    D_8009D1D0 = 0;

    if ((mode & 0xFF) == 1) {
        D_8009D234[0] = 0x5A;
        D_8009D244 = 1;
        asm volatile("" ::: "memory");
        flags = actor->stateFlags;
        flags |= 0x800000;
    } else {
        flags = actor->stateFlags;
        D_8009D244 = 0;
        flags &= 0xFFBFFFFF;
        flags &= 0xFF7FFFFF;
    }
    actor->stateFlags = flags;
    asm volatile("" ::: "memory");

    clear_actor = D_8009D278;
    flags = clear_actor->stateFlags;
    clear_actor->panelA_timer = 0;
    flags &= -4;
    flags &= -0xD;
    flags &= -0x31;
    flags &= -0xC1;
    flags &= -0x101;
    flags &= -0x201;
    flags &= -0x401;
    flags &= -0x801;
    flags &= -0x1001;
    flags &= -0x2001;
    flags &= -0x4001;
    flags &= 0xFFFF7FFF;
    flags &= 0xFFF7FFFF;
    flags &= 0xFFFEFFFF;
    flags &= 0xFFF9FFFF;
    flags &= 0xFFEFFFFF;
    flags &= 0xFFDFFFFF;
    flags &= 0xFEFFFFFF;
    flags &= 0xF1FFFFFF;
    flags &= 0xEFFFFFFF;
    flags &= 0xDFFFFFFF;
    clear_actor->stateFlags = flags;

    D_8009D278->panelB_timer = 0;
    D_8009D278->panelAux_timer = 0;
    {
        u32 state;
        state = reset_flags_read[0];
        reset_flags_write[0] = state & ~0x10;
    }
    Battle_FlushScriptSounds();
    Tbl_ResetAll();
    D_8009D298[0] = 0;
    D_8009D29A[0] = 0;
    D_8009D29B[0] = 0;
    D_8009D29C[0] = 0;
}
