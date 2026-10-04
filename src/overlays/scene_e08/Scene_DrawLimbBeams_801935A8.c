#include "pe1/scene_player_orb.h"

/* Draw the ten limb beams that are switched on (mode 1). */
void func_801935A8(void *object, void *timer, SceneLimbBeams *beams)
{
    GteShortVector points[10];
    RenderColor head;
    RenderColor tail;
    ScenePlayerOrbAnchor *anchor;
    RenderMatrix *matrices;
    RenderMatrix *target;
    int mode;
    u32 i;
    int tpage;

    anchor = func_800C2B50();
    target = &g_PlayerEntity->render_object.matrices[2];
    func_800C2EAC(anchor->mode);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
    D_800F3368.depth = 0x1F4;
    for (i = 0; i < 10; i++) {
        mode = beams->mode[i];
        if (mode == 1) {
            matrices = g_PlayerEntity->render_object.matrices;
            if (i == 0) {
                target = &matrices[18];
            }
            if (i == 1) {
                target = &matrices[24];
            }
            if (i == 2) {
                target = &matrices[28];
            }
            if (i == 3) {
                target = &matrices[24];
            }
            if (i == 4) {
                target = &matrices[28];
            }
            points[0].x = matrices[2].translation[0] + (func_80077CF4(D_80199658[i] + D_801994D8[i].swing) >> 5);
            points[0].y = matrices[2].translation[1];
            points[0].z = matrices[2].translation[2] + (func_80077DC4(D_80199658[i] + D_801994D8[i].swing) >> 5);
            points[1].x = D_801994D8[i].rise + matrices[2].translation[0];
            points[1].y = D_801994D8[i].rise + matrices[2].translation[1];
            points[1].z = D_801994D8[i].rise + matrices[2].translation[2];
            points[2].x = D_801994D8[i].bend + ((matrices[2].translation[0] + target->translation[0]) >> 1);
            points[2].y = D_801994D8[i].bend + ((matrices[2].translation[1] + target->translation[1]) >> 1);
            points[2].z = D_801994D8[i].bend + ((matrices[2].translation[2] + target->translation[2]) >> 1);
            points[3].x = D_801994D8[i].reach + target->translation[0];
            points[3].y = D_801994D8[i].reach + target->translation[1];
            points[3].z = D_801994D8[i].reach + target->translation[2];
            points[4].x = target->translation[0];
            points[4].y = target->translation[1];
            points[4].z = target->translation[2];
            func_800C3134(D_801989BC, beams->fade[i], &head);
            func_800C3134(D_801989D0, beams->fade[i], &tail);
            tpage = (u16)func_80077A64(0, 0, 0x340, 0x100);
            func_800D3114(points, 4, 100, 0x40, 0x80, 0x9C, 0x10, tpage,
                          (u16)func_80077AA4(0, 0x1D7), 0x80, &tail, &tail, mode);
        }
    }
}
