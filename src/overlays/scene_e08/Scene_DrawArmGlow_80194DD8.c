#include "pe1/scene_player_orb.h"

/* Draw a glow sprite on the player's hand and elbow bones, then a ribbon
 * bent between them through the five script offsets. */
void func_80194DD8(void *object, void *timer, SceneArmGlow *glow)
{
    GteMatrix matrix;
    GteShortVector points[7];
    RenderColor head;
    RenderColor tail;
    GteVector scale;
    GteVector source;
    GteVector second;
    ScenePlayerOrbAnchor *anchor;
    RenderMatrix *matrices;
    int tpage;

    anchor = func_800C2B50();
    matrices = anchor->actor->render_object.matrices;
    head = D_8018F060;
    tail = D_8018F064;
    func_800C2EAC(anchor->mode);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
    D_800F3368.depth = 0x1F4;
    D_801994C8.frame = glow->frame;
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
    func_80071A44(&source, 0, 0x10);
    source.x = glow->scale;
    source.y = glow->scale;
    source.z = 0x1000;
    scale = source;
    func_80078CC4(&matrix, &scale);
    matrix.t[0] = matrices[32].translation[0];
    matrix.t[1] = matrices[32].translation[1];
    matrix.t[2] = matrices[32].translation[2];
    func_800C42A4(&D_801994C8, &matrix, 1);
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
    func_80071A44(&second, 0, 0x10);
    second.x = glow->scale;
    second.y = glow->scale;
    second.z = 0x1000;
    source = second;
    func_80078CC4(&matrix, &source);
    matrix.t[0] = matrices[22].translation[0];
    matrix.t[1] = matrices[22].translation[1];
    matrix.t[2] = matrices[22].translation[2];
    func_800C42A4(&D_801994C8, &matrix, 1);
    points[0].x = matrices[32].translation[0];
    points[0].y = matrices[32].translation[1];
    points[0].z = matrices[32].translation[2];
    points[1].x = glow->offsets[0][0] + matrices[32].translation[0];
    points[1].y = glow->offsets[0][1] + matrices[32].translation[1];
    points[1].z = glow->offsets[0][2] + matrices[32].translation[2];
    points[2].x = glow->offsets[1][0] + matrices[32].translation[0];
    points[2].y = glow->offsets[1][1] + matrices[32].translation[1];
    points[2].z = glow->offsets[1][2] + matrices[32].translation[2];
    points[3].x = glow->offsets[4][0] + ((matrices[32].translation[0] + matrices[22].translation[0]) >> 1);
    points[3].y = glow->offsets[4][1] + ((matrices[32].translation[1] + matrices[22].translation[1]) >> 1);
    points[3].z = glow->offsets[4][2] + ((matrices[32].translation[2] + matrices[22].translation[2]) >> 1);
    points[4].x = glow->offsets[2][0] + matrices[22].translation[0];
    points[4].y = glow->offsets[2][1] + matrices[22].translation[1];
    points[4].z = glow->offsets[2][2] + matrices[22].translation[2];
    points[5].x = glow->offsets[3][0] + matrices[22].translation[0];
    points[5].y = glow->offsets[3][1] + matrices[22].translation[1];
    points[5].z = glow->offsets[3][2] + matrices[22].translation[2];
    points[6].x = matrices[22].translation[0];
    points[6].y = matrices[22].translation[1];
    points[6].z = matrices[22].translation[2];
    tpage = (u16)func_80077A64(0, 0, 0x340, 0x100);
    func_800D3114(points, 7, 300, 0x40, 0x80, 0x9C, 0x10, tpage,
                  (u16)func_80077AA4(0, 0x1D7), 0x80, &head, &tail, 1);
}
