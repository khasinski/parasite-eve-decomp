#include "pe1/battle.h"

extern int g_FieldMoveLock;
extern char *g_ActiveActor;
extern char *g_PlayerEntity;

void Entity_SetActionMode(char *arg0, int arg1);

#define COMBATANT_FIELD(ptr, type, member) \
    (*(type *)((ptr) + PE1_OFFSETOF(Combatant, member)))
#define ENTITY_FIELD(ptr, type, member) \
    (*(type *)((ptr) + PE1_OFFSETOF(BattleEntity, member)))

void Battle_ReturnToIdle(void) {
    char *state;
    char *actor;
    int mask_state;
    int flags;

    mask_state = 0xFFFEFFFF;
    g_FieldMoveLock &= -2;
    state = g_ActiveActor;
    actor = g_PlayerEntity;
    flags = COMBATANT_FIELD(state, int, stateFlags);
    flags &= mask_state;
    COMBATANT_FIELD(state, int, stateFlags) = flags;
    ENTITY_FIELD(actor, int, motionX) = 0;
    ENTITY_FIELD(actor, int, motionY) = 0;
    ENTITY_FIELD(actor, int, motionZ) = 0;
    Entity_SetActionMode(actor, COMBATANT_FIELD(state, unsigned char, actionMode12));
}

#undef ENTITY_FIELD
#undef COMBATANT_FIELD
