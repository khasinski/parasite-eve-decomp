#include "pe1/psyq_gpu.h"
#include "pe1/render_packets.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */


#include "include_asm.h"
void DrawSync(int arg0);
void Menu_DrawSaveBg(void);
int VSync(int mode);
int Seq_GetElapsed(void);
void SetDispMask(int arg0);
void ResetGraph(int arg0);
int Gpu_CheckDrawStatus(void);

extern int g_GameState[];
extern int g_ActiveDrawSlot;
extern DISPENV g_RenderDispEnvArray[];
extern DRAWENV g_RenderDrawEnvArray[];

#define D_800B0CD8_WORD (g_GameState[0])

static inline DISPENV *DispAddress(int index) {
    return &g_RenderDispEnvArray[index];
}

static inline DRAWENV *DrawAddress(int index) {
    return &g_RenderDrawEnvArray[index];
}

void Gpu_RenderFrame(void) {
    int idx;
    int state;
    int status;
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

    ResetGraph(1);

    idx = g_ActiveDrawSlot;
    PutDispEnv(DispAddress(idx));

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
    PutDrawEnv(DrawAddress(idx));
    goto done;

draw_buffer:
    {
        int drawSlot;
        char *bufferEntry;
        int pointerOffset;
        char *orderingTable;

        drawSlot = g_ActiveDrawSlot;
        pointerOffset = drawSlot << 2;
        asm volatile("" : "=r"(state_ptr) : "0"(state_ptr));
        bufferEntry = (char *)state_ptr + pointerOffset;
        orderingTable = ((RenderBufferPrefix *)(bufferEntry + 0x160))->ordering[0];
        DrawOTagEnv(orderingTable + 0x3FFC, &g_RenderDrawEnvArray[drawSlot]);
    }

done:
    g_ActiveDrawSlot = g_ActiveDrawSlot < 1U;
}
