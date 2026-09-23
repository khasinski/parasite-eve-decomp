/* CC1_FLAGS: -fno-strength-reduce */
#include "pe1/geom_state.h"
#include "pe1/render_prim.h"
#include "pe1/render_camera.h"
extern short D_800BD028, D_800BD02A;
extern u16 D_800BCFAC, D_800BCFAE, D_800BCFB0, D_800BCFB2;
extern u8 D_800BCFFA, D_800BCFFB;
int Gpu_LoadGeomState(int);
int Geo_RenderMeshList(void *buffer, void **end)
{
    s16 *minY;
    /* Reserve the otherwise unused slot in the retail 56-byte stack frame. */
    char frame_pad[2];
    u16 highY;
    u16 lowY;
    GeomState *state = D_800B1624;
    register u8 *group asm("$17") = &g_GeomGroupSel;
    CameraViewport *view;
    GeomEntry *entries;
    register unsigned int count asm("$19");
    unsigned int i;
    register int x asm("$4");
    Gpu_LoadGeomState(*group);
    view = (CameraViewport *)((u8 *)state + state->entry_offset_1C) + *group;
    x = (view->minX + view->maxX) / 2;
    {
        register short centerX = x;
        asm("" : "=r"(centerX) : "0"(centerX));
        D_800BD028 = centerX;
        D_800BCF8C.x = centerX;
    }
    minY = &view->minY;
    D_800BCF8E = D_800BD02A = (*minY + view->maxY) / 2;
    asm volatile("":::"memory");
    D_800BCFAC = view->minX;
    asm volatile("":::"memory");
    D_800BCFAE = view->maxX;
    asm volatile("":::"memory");
    lowY = *minY;
    i = 0;
    D_800BCFB0 = lowY;
    asm volatile("":::"memory");
    highY = view->maxY;
    x -= 160;
    D_800BCFB2 = highY;
    asm volatile("":::"memory");
    count = state->entry_count06;
    state->out_disp_x = state->disp_src_x - x;
    asm("" ::: "memory");
    state->out_disp_y = state->disp_src_y - (D_800BCF8E - 112);
    entries = (GeomEntry *)((u8 *)state + state->entry_offset);
    *end = buffer;
    if (count) {
        int min = -32768, max = 32767;
        GeomEntry *entry = entries;
        do {
            if (Geo_LoadMeshEntry(entry, *end, end)) return -18;
            entry->ot10 = min;
            entry->ot12 = max;
            entry->ot14 = min;
            entry->ot16 = max;
            entry++;
        } while (++i < count);
    }
    D_800BCFFA = D_800BCFFB = 0;
    *(volatile u32 *)&D_800BCF88.flags &= ~0xc00;
    return 0;
}
