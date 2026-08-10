#include "common.h"
#include "pe1/battle.h"

extern volatile char *g_ActiveActor;
extern volatile char *g_PlayerEntity;

#define COMBATANT_FIELD(ptr, type, member) \
    (*(volatile type *)((ptr) + PE1_OFFSETOF(Combatant, member)))
#define ENTITY_FIELD(ptr, type, member) \
    (*(volatile type *)((ptr) + PE1_OFFSETOF(BattleEntity, member)))

void Battle_SubActionStep(void) {
    volatile char *state;

    state = g_ActiveActor;
    if (COMBATANT_FIELD(state, u8, subActionCounter) >=
        COMBATANT_FIELD(state, u8, subActionPeriod)) {
        register volatile char *actor asm("$4");
        register volatile char *state_v1 asm("$3");
        u16 value;
        register u16 step asm("$3");
        u16 copy;

        value = COMBATANT_FIELD(state, u16, curHP);
        step = COMBATANT_FIELD(state, u16, subActionStep);
        copy = COMBATANT_FIELD(state, u16, subActionStep);
        value -= step;
        COMBATANT_FIELD(state, u16, curHP) = value;
        value = COMBATANT_FIELD(state, u16, hpMirror);
        step = COMBATANT_FIELD(state, u16, subActionStep);
        COMBATANT_FIELD(state, u8, subActionCounter) = 0;
        COMBATANT_FIELD(state, u16, panelAux_val) = copy;
        actor = g_PlayerEntity;
        value -= step;
        state_v1 = g_ActiveActor;
        COMBATANT_FIELD(state, u16, hpMirror) = value;
        value = ENTITY_FIELD(actor, u16, renderObject.projected_x);
        COMBATANT_FIELD(state_v1, u16, panelAux_x) = value;
        copy = ENTITY_FIELD(actor, u16, renderObject.projected_y);
        COMBATANT_FIELD(state_v1, u8, panelAux_timer) = 0x1E;
        COMBATANT_FIELD(state_v1, u16, panelAux_y) = copy;
        state_v1 = g_ActiveActor;
        COMBATANT_FIELD(state_v1, u8, panelAux_mode) = 3;
    }

    {
        Combatant *state_v1;
        u8 value;

        state_v1 = (Combatant *)g_ActiveActor;
        value = *(u8 *)&state_v1->subActionCounter;
        value++;
        *(u8 *)&state_v1->subActionCounter = value;
    }
}

#undef ENTITY_FIELD
#undef COMBATANT_FIELD
