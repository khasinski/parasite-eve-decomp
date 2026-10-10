#include "pe1/draw_buffers.h"
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

extern RenderFrameState g_GameState;
extern int g_ActiveDrawSlot;


static inline DISPENV *DispAddress(int index) {
    return &g_RenderDispEnvArray[index];
}

static inline DRAWENV *DrawAddress(int index) {
    return &g_RenderDrawEnvArray[index];
}

void Gpu_RenderFrame(void) {
    RenderBufferPrefix *buffers;
    int displayFlags;
    int idx;
    int state;
    int status;
    RenderFrameState *state_ptr;

    DrawSync(0);
    Menu_DrawSaveBg();

    displayFlags = g_GameState.flags & 0x200;
    if (displayFlags) {
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

    state_ptr = &g_GameState;
    state = state_ptr->flags;
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
        char *orderingTable;

        drawSlot = g_ActiveDrawSlot;
        buffers = &state_ptr->buffers;
        orderingTable = buffers->ordering[drawSlot];
        DrawOTagEnv(orderingTable + 0x3FFC, &g_RenderDrawEnvArray[drawSlot]);
    }

done:
    g_ActiveDrawSlot = g_ActiveDrawSlot < 1U;
}
