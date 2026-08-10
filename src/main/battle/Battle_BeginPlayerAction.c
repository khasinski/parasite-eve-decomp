#include "pe1/battle.h"

extern int g_EntityControlFlags __asm__("D_8009D2E8");
extern char *g_PlayerEntity __asm__("D_8009D254");
extern char *g_ActiveActor __asm__("D_8009D278");

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
    register int flags asm("$2");

    mask = -0x101;
    g_EntityControlFlags |= 1;
    player = g_PlayerEntity;
    flags = ENTITY_FIELD(player, int, entityFlags);
    actor = g_ActiveActor;
    flags &= mask;
    ENTITY_FIELD(player, int, entityFlags) = flags;
    flags = COMBATANT_FIELD(actor, int, stateFlags);
    flags |= 0x10000;
    COMBATANT_FIELD(actor, int, stateFlags) = flags;
    asm("sh $0,%gp_rel(D_8009D298)($28)");
    Entity_SetActionMode(player, 0x12);
    Battle_FlushScriptSounds();
}

#undef ENTITY_FIELD
#undef COMBATANT_FIELD
