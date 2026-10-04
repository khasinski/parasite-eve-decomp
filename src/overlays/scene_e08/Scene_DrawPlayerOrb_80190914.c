#include "pe1/scene_player_orb.h"

/* Draw the orb's two fading ribbons and the eight trail sprites. */
void func_80190914(void *object, void *timer, ScenePlayerOrb *orb)
{
    RenderColor color;
    GteMatrix matrix;
    GteVector scale;
    ScenePlayerOrbAnchor *anchor;
    u32 i;

    anchor = func_800C2B50();
    color = D_8018F004;
    func_800C2EAC(anchor->mode);
    func_800C3098(0x10);
    func_800C3238(2);
    func_800C2FF0(0x10, 0x10);
    if (orb->active != 0) {
        func_800C3134(D_801987E4, orb->fade, &orb->head);
        func_800C3134(D_801987E4, orb->fade, &orb->tail);
        func_800D3114(orb->trailA, orb->count - 1, 0x3C, 0x20, 0x90, 0x51, 0x10,
                      (u16)func_80077A64(1, 0, 0x340, 0x100), (u16)func_80077AA4(0, 0x1D9),
                      orb->alpha, &orb->head, &orb->tail, 1);
        func_800D3114(orb->trailB, orb->count - 1, 0x3C, 0x20, 0x90, 0x51, 0x10,
                      (u16)func_80077A64(1, 0, 0x340, 0x100), (u16)func_80077AA4(0, 0x1D9),
                      orb->alpha, &color, &color, 2);
        matrix.m[2][2] = 0x1000;
        matrix.m[1][1] = 0x1000;
        matrix.m[0][0] = 0x1000;
        matrix.t[2] = 0;
        matrix.t[1] = 0;
        matrix.t[0] = 0;
        matrix.m[2][1] = 0;
        matrix.m[2][0] = 0;
        matrix.m[1][2] = 0;
        matrix.m[1][0] = 0;
        matrix.m[0][2] = 0;
        matrix.m[0][1] = 0;
        scale = D_8018F008;
        func_80078CC4(&matrix, &scale);
        for (i = 0; i < 8; i++) {
            matrix.t[0] = orb->points[i].x;
            matrix.t[1] = orb->points[i].y;
            matrix.t[2] = orb->points[i].z;
            func_800C42A4(D_80199690, &matrix, 1);
        }
    }
}
