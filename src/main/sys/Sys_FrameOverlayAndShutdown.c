/* End-of-frame overlay pass (draw the game-state primitives into the
 * current draw buffer with background clear off and send the queued AKAO
 * commands) and the soft-reset shutdown; contiguous default-profile pair
 * that Boot_RunFrame calls back to back. */
#include "common.h"
#include "pe1/psyq_gpu.h"
#include "pe1/game_state.h"

extern DRAWENV g_RenderDrawEnvArray[2];
extern s32 g_ActiveDrawSlot;
extern int g_SceneDispatchToken;
extern int D_800A77F4;

void Akao_Cmd_98_9A_9C(int arg0);
void Akao_Cmd_99_9B_9D(int arg0);
void DrawPrim(void *prim);
DRAWENV *PutDrawEnv(DRAWENV *env);
void DsReadBreak(void);
void Akao_Cmd_D8(int arg0);
void Akao_Cmd_F0(void);
void Akao_Cmd_F1(void);
void Spu_Shutdown(void);
int Menu_IsEquipSlotActive(void);
int Menu_ResetEquipSlotState(void);

void Akao_StepVoiceTable(void) {
    s32 one;

    if (!(g_GameState.flags & 0x200) && (g_GameStateFlags & 0x10)) {
        one = 1;
        g_RenderDrawEnvArray[g_ActiveDrawSlot].isbg = 0;
        g_RenderDrawEnvArray[g_ActiveDrawSlot].dfe = one;
        PutDrawEnv(&g_RenderDrawEnvArray[g_ActiveDrawSlot]);
        DrawPrim(g_GameState.draw_prim_c);
        DrawPrim(g_GameState.draw_prim_b);
        g_RenderDrawEnvArray[g_ActiveDrawSlot].isbg = one;
        g_RenderDrawEnvArray[g_ActiveDrawSlot].dfe = 0;
        Akao_Cmd_99_9B_9D(0);
    }
    if (g_GameStateFlags & 0x20) {
        Akao_Cmd_98_9A_9C(0);
    }
}

void Sys_Shutdown(void) {
    int old;

    DsReadBreak();
    Akao_Cmd_D8(0);
    Akao_Cmd_F0();
    Akao_Cmd_F1();
    Spu_Shutdown();

    if (Menu_IsEquipSlotActive() & 0xFF) {
        Menu_ResetEquipSlotState();
    }

    old = g_SceneDispatchToken;
    g_SceneDispatchToken = 0xA9400048;
    D_800A77F4 = old;
    g_GameState.flags |= 0x100;
}
