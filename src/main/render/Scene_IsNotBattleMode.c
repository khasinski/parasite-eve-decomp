/* MASPSX_FLAGS: --expand-div */
#include "pe1/geom_state.h"
#include "pe1/gte.h"

/* Advance the camera transition, then center the selected viewport. */
int Scene_IsNotBattleMode(void)
{
    unsigned int flags = g_RenderStateFlags;
    int mode, smooth;
    u16 dx, dy, duration;
    if (!(flags & 0x40))
        return -20;
    mode = flags & 7;
    smooth = flags & 8;
    if (mode != 1 && mode != 2 && mode != 3)
        return 0;
    {
        u16 targetX = D_800BCF9C, x = D_800BCF98;
        u16 targetY = D_800BCF9E, y = D_800BCF9A;
        u16 time = D_800BCFA0;
        duration = D_800BCFA2;
        dx = targetX - x;
        dy = targetY - y;
        if (!smooth) {
            int movedX = (short)dx * (short)time / (short)duration;
            int movedY = (short)dy * (short)time / (short)duration;
            D_800BCF8C.x = x + movedX;
            D_800BCF8E = y + movedY;
        } else {
            int weight = rcos((short)time * 2048 / (short)duration + 2048) + 4096;
            int movedX = (short)dx * weight / 8192;
            int movedY = (short)dy * weight / 8192;
            D_800BCF8C.x = D_800BCF98 + movedX;
            D_800BCF8E = D_800BCF9A + movedY;
        }
    }
    {
        GeomState *state = D_800B1624;
        CameraViewport *views = (CameraViewport *)((u8 *)D_800B1624 + state->entry_offset_1C);
        CameraViewport *view = &views[g_GeomGroupSel];
        {
            unsigned int offset = 160 - (D_800BCF8C.x - (short)view->width / 2);
            D_800BCF94 = offset;
        }
        {
            unsigned int offset = 112 - ((short)D_800BCF8E - (short)view->height / 2);
            D_800BCF96 = offset;
        }
    }
    if (mode == 1)
        g_RenderStateFlags = (g_RenderStateFlags & ~7) | 2;
    {
        u16 time = D_800BCFA0 + 1;
        D_800BCFA0 = time;
        if ((short)time > (short)duration) {
            unsigned int completed = g_RenderStateFlags;
            if ((completed & 7) == 2)
                g_RenderStateFlags = (completed & ~7) | 0x84;
            else
                g_RenderStateFlags = (completed & ~7) | 0x80;
        }
    }
    return 0;
}
