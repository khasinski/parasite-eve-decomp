#include "pe1/battle.h"
/* CC1_FLAGS: -G2 */
/* MASPSX_FLAGS: -G2 */

extern int D_8009D2E8;
extern char *D_8009D254;
extern char *D_8009D278;
extern short D_8009D298;

void Entity_SetActionMode(char *arg0, int arg1);
void Battle_FlushScriptSounds(void);

#define COMBATANT_FIELD(ptr, type, member) \
    (*(type *)((ptr) + PE1_OFFSETOF(Combatant, member)))
#define ENTITY_FIELD(ptr, type, member) \
    (*(type *)((ptr) + PE1_OFFSETOF(BattleEntity, member)))

void Battle_BeginPlayerAction(void) {
    char *player;
    char *actor;
    int mask;
    int flags;

    mask = -0x101;
    D_8009D2E8 |= 1;
    player = D_8009D254;
    flags = ENTITY_FIELD(player, int, entityFlags);
    actor = D_8009D278;
    flags &= mask;
    ENTITY_FIELD(player, int, entityFlags) = flags;
    COMBATANT_FIELD(actor, int, stateFlags) |= 0x10000;
    D_8009D298 = 0;
    Entity_SetActionMode(player, 0x12);
    Battle_FlushScriptSounds();
}

#undef ENTITY_FIELD
#undef COMBATANT_FIELD

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
