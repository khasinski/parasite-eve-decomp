/* Script opcodes: test, set and clear the global game-state flags, set the
 * battle mode state and read a battle context field of the current actor.
 * Contiguous default-profile handlers. */
#include "pe1/field_actor.h"

extern int g_GameStateFlags;
extern int g_BattleModeState;
extern FieldActor *g_CurrentEntity;

void *Battle_GetContextField(int arg0);
void *Battle_GetEnemyContextField(void *arg0, int arg1);

int Scene_TestGlobalFlags(int **arg0) {
    int flags;
    int mask;

    flags = g_GameStateFlags;
    mask = *arg0[0];
    if (flags & mask) {
        *arg0[1] = 1;
    } else {
        *arg0[1] = 0;
    }
    return 1;
}

int Scene_SetGlobalFlags(int **arg0) {
    g_GameStateFlags |= *arg0[0];
    return 1;
}

int Scene_ClearGlobalFlags(int **arg0) {
    g_GameStateFlags &= ~*arg0[0];
    return 1;
}

int Menu_SetModeState5(void) {
    g_BattleModeState = 5;
    return 1;
}

int Menu_SetModeState6(void) {
    g_BattleModeState = 6;
    return 1;
}

int Entity_GetField(char ***arg0) {
    register char *value asm("$2");
    FieldActor *ctx = g_CurrentEntity;

    if (ctx->type_id == 0) {
        value = Battle_GetContextField(*(unsigned char *)arg0[0]);
    } else {
        value = Battle_GetEnemyContextField(ctx, *(unsigned char *)arg0[0]);
    }

    *arg0[1] = (char *)value;
    return 1;
}
