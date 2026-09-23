/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "pe1/battle_start.h"

void Battle_StartEncounter(int mode) {
    register int i asm("$4");
    register int neg_one asm("$5");
    BattleEntity *entity;
    Combatant *actor;
    u32 value;
    int mode_reg;
    entity = D_8009D254[0];
    D_8009D278 = (Combatant *)entity->core;
    D_8009D1E8 = 0;
    (*(u32 *)&battle_start_phase_storage) = 0;
    D_8009D28C = 0;
    D_8009CE7C = 0;
    D_8009CE78 = 0;
    D_8009D288 = 0;
    D_8009CE74 = 0;
    mode_reg = mode;
    Save_ResetGlobalFlags();

    i = 0;
    neg_one = -1;
    battle_start_flags_byte[0] = 0;
    asm volatile("" : : : "memory");
    { register u32 initial_flags asm("$3");
    initial_flags = battle_start_flags_read[0];
    D_8009D1A8[0] = 0;
    D_8009D1CE[0] = 0;
    D_8009D235[0] = 0;
    D_8009D304 = 0;
    D_8009D21C = 0;
    battle_start_flags_write[0] = initial_flags & -0x301;
    }

    for (; (u8)i < 0xA; i++) {
        D_800A7FF0[(u8)i].id = 0;
        D_800A7FF0[(u8)i].extra = neg_one;
    }

    for (i = 0; (u8)i < 7; i++) {
        D_800B8A90[(u8)i] = 0;
    }

    srand((*(u32 *)&battle_start_seed_storage));
    Battle_ResetEnemyStats(0);
    Battle_SetupEnemyAnims();

    actor = D_8009D278;
    value = actor->exp_or_acc;
    if ((s32)value <= 0) {
        actor->exp_or_acc = 0x10000;
    } else {
        u32 max_value;

        max_value = actor->maxAtk;
        if ((s32)value >= (s32)max_value) {
            actor->exp_or_acc = max_value;
            actor->atbGauge = 0xF0;
        }
    }

    Battle_CheckDropChance();
    {
        BattleEntity *post_entity;
        RenderMatrix *post_actor;
        u32 callback;
        post_entity = D_8009D254[0];
        post_actor = post_entity->renderObject.matrices;
        callback = (u32)Entity_CheckActionIdMatch;
        post_entity->actionCheckFn = (void *)callback;
        (*(s16 *)&battle_start_camera_storage) = post_actor->translation[1] - 0x64;
    }
    Window_SetBoundsByMode(mode_reg & 0xFF);
    {
        Combatant *final_actor;
        void *final_entity;
        final_actor = D_8009D278;
        final_entity = battle_start_final_entity[0];
        Entity_SetActionMode(final_entity, final_actor->actionMode12);
    }
}
