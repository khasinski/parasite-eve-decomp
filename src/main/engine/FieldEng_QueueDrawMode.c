#include "pe1/render_prim.h"

static __inline__ void link_packet(void *ordering, void *packet)
{
    *(u32 *)packet = (*(u32 *)packet & 0xff000000) | (*(u32 *)ordering & 0xffffff);
    *(u32 *)ordering = (*(u32 *)ordering & 0xff000000) | ((u32)packet & 0xffffff);
}
void func_800CF6F8(void *ordering, void *packet, int mode)
{
    void *drawMode;
    if (mode != 255) {
        drawMode = D_800B0E38.packets[D_8009CDDC] + D_8009CDD8;
        D_8009CDD8 += 8;
        SetDrawMode(drawMode, 0, 1, GetTPage(0, mode, 0, 0));
        if (packet) {
            ((u8 *)packet)[7] |= 2;
            link_packet(ordering, packet);
        }
        link_packet(ordering, drawMode);
    } else if (packet) {
        link_packet(ordering, packet);
    }
}
