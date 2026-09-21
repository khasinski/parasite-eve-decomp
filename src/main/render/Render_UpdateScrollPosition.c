#include "pe1/geom_state.h"
#include "pe1/gte.h"
#include "pe1/psyq_nop.h"

/* Project a world-space point and start a clamped camera transition. */
int Render_UpdateScrollPosition(void *positionArg, int duration, int mode)
{
    int *position = positionArg;
    short vector[4];
    short projected[2];
    GeomScrollState *camera = &D_800BCF88;
    CameraViewport *view;
    int x, y;

    if (!(camera->flags & 64))
        return -21;
    D_800BCF98 = D_800BCF8C.x;
    D_800BCF9A = D_800BCF8E;
    asm volatile("" : : : "memory");
    vector[0] = position[0]>>16;
    vector[1] = (position[1]>>16)-(u16)D_800BCFFE;
    vector[2] = position[2]>>16;
    {
        /* Narrow volatile register retains GCC's retail 24-byte frame.
         * This is a matching constraint, not evidence of the original type. */
        register volatile unsigned short cx asm("$10") = 160;
        register unsigned int cy asm("$11") = 112;
        register unsigned int sx asm("$12");
        register unsigned int sy asm("$13");
        asm("" : "=r"(cx), "=r"(cy) : "0"(cx), "1"(cy) : "memory");
        sx = cx<<16;
        sy = cy<<16;
        gte_ctc2_24(sx);
        gte_ctc2_25(sy);
    }
    {
        register unsigned int **address asm("$2") = &camera->position.matrixWords;
        register unsigned int *matrix asm("$10");
        register unsigned int a asm("$12");
        register unsigned int b asm("$13");
        register unsigned int c asm("$14");
        asm("" : "=r"(address) : "0"(address));
        matrix = *address;
        a = matrix[0];
        b = matrix[1];
        asm("" : : "r"(a), "r"(b));
        gte_ctc2_0(a);
        gte_ctc2_1(b);
        a = matrix[2];
        b = matrix[3];
        c = matrix[4];
        asm("" : : "r"(a), "r"(b), "r"(c));
        gte_ctc2_2(a);
        gte_ctc2_3(b);
        gte_ctc2_4(c);
        a = matrix[5];
        b = matrix[6];
        asm("" : : "r"(a), "r"(b));
        gte_ctc2_5(a);
        c = matrix[7];
        gte_ctc2_6(b);
        gte_ctc2_7(c);
    }
    gte_lwc2_0_0(vector);
    gte_lwc2_1_4(vector);
    PE1_NOP();
    PE1_NOP();
    gte_rtps_command();
    gte_stsxy2(projected);
    {
        GeomState *state = D_800B1624;
        CameraViewport *views = (CameraViewport *)((u8 *)D_800B1624+state->entry_offset_1C);
        view = &views[g_GeomGroupSel];
    }
    {
        u16 width = view->width,height = view->height;
        int halfX = (short)width/2-160,halfY = (short)height/2-112;
        x = halfX+projected[0];
        y = halfY+projected[1];
    }
    {
        int min = view->minX;
        if (x < min) x = min;
        else {
            int max = view->maxX;
            if (x > max) x = max;
        }
    }
    {
        int min = view->minY;
        if (y < min) y = min;
        else {
            int max = view->maxY;
            if (y > max) y = max;
        }
    }
    D_800BCF9C = x;
    D_800BCF9E = y;
    if (duration == -1)
        D_800BCFA2 = 30;
    else
        D_800BCFA2 = duration;
    D_800BCFA0 = 1;
    if (mode == -1)
        g_RenderStateFlags = (g_RenderStateFlags & ~7) | 3;
    else {
        unsigned int flags = g_RenderStateFlags & ~15;
        unsigned int bits = mode | 3;
        g_RenderStateFlags = flags | bits;
    }
    return 0;
}
