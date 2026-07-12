/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */


#include "include_asm.h"
void DrawSync(int arg0);
void Menu_DrawSaveBg(void);
void VSync(int arg0);
int Seq_GetElapsed(void);
void SetDispMask(int arg0);
void Render_InitEntityPool(int arg0);
void PutDispEnv(void *arg0) __asm__("Render_StepEntityPool");
int Gpu_CheckDrawStatus(void);
void PutDrawEnv(void *arg0);
void Gpu_PutDrawEnvLinked(int arg0, void *arg1);

extern int g_GameState[];
extern int g_ActiveDrawSlot;
extern char g_RenderDispEnvArray[];
extern char g_RenderDrawEnvArray[];

#define D_800B0CD8_WORD (g_GameState[0])

void Gpu_RenderFrame(void) {
    register int idx asm("$2");
    int state;
    int status;
    register int offset asm("$4");
    int *state_ptr;

    DrawSync(0);
    Menu_DrawSaveBg();

    if (D_800B0CD8_WORD & 0x200) {
        VSync(4);
        if ((short)Seq_GetElapsed() >= 3) {
            SetDispMask(1);
        }
    } else {
        VSync(2);
    }

    Render_InitEntityPool(1);

    {
        char *base;

        idx = g_ActiveDrawSlot;
        offset = idx << 2;
        offset += idx;
        offset <<= 2;
        base = g_RenderDispEnvArray;
        PutDispEnv(base + offset);
    }

    status = Gpu_CheckDrawStatus();
    if ((status << 24) != 0) {
        goto draw_direct;
    }

    state_ptr = g_GameState;
    state = *state_ptr;
    if ((state & 0x200) == 0) {
        goto draw_buffer;
    }

draw_direct:
    idx = g_ActiveDrawSlot;
    offset = (idx << 1) + idx;
    offset <<= 3;
    offset -= idx;
    offset <<= 2;
    PutDrawEnv(g_RenderDrawEnvArray + offset);
    goto done;

draw_buffer:
    {
        int idx_b;
        register int tmp asm("$2");
        int arg0;
        int arg1;
        char *base;

        idx_b = g_ActiveDrawSlot;
        tmp = idx_b << 2;
        asm("addu %0,%1,%2" : "=r"(tmp) : "r"(state_ptr), "r"(tmp));
        arg1 = (idx_b << 1) + idx_b;
        arg1 <<= 3;
        arg1 -= idx_b;
        arg1 <<= 2;
        arg0 = *(int *)(tmp + 0x160);
        base = g_RenderDrawEnvArray;
        arg1 += (int)base;
        Gpu_PutDrawEnvLinked(arg0 + 0x3FFC, (void *)arg1);
    }

done:
    g_ActiveDrawSlot = g_ActiveDrawSlot < 1U;
}
