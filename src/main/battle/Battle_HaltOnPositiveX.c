#include "pe1/battle.h"

extern char *g_ActiveActor;
extern char *g_PlayerEntity;

void Entity_SetActionMode(char *arg0, int arg1);
void Battle_FlushScriptSounds(char *arg0);

#define COMBATANT_FIELD(ptr, type, member) \
    (*(type *)((ptr) + PE1_OFFSETOF(Combatant, member)))
#define ENTITY_FIELD(ptr, type, member) \
    (*(type *)((ptr) + PE1_OFFSETOF(BattleEntity, member)))

void Battle_HaltOnPositiveX(void) {
    char *state;
    char *actor;
    char *state2;
    char *actor2;
    int flags;

    state = g_ActiveActor;
    if (COMBATANT_FIELD(state, s16, curHP) > 0) {
        actor = g_PlayerEntity;
        ENTITY_FIELD(actor, int, motionX) = 0;
        ENTITY_FIELD(actor, int, motionY) = 0;
        ENTITY_FIELD(actor, int, motionZ) = 0;
        Entity_SetActionMode(actor, 0x12);

        state2 = g_ActiveActor;
        actor2 = g_PlayerEntity;
        flags = COMBATANT_FIELD(state2, int, stateFlags);
        asm volatile("sb $0, 0xC0($gp)");
        flags |= 0x2000;
        COMBATANT_FIELD(state2, int, stateFlags) = flags;
        ENTITY_FIELD(actor2, int, entityFlags) &= ~0x100;
        Battle_FlushScriptSounds(actor2);
    }
}

#undef ENTITY_FIELD
#undef COMBATANT_FIELD
