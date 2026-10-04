#include "pe1/scene_glow_model.h"

/* Draw the scene model with the owner's texture page, then a glow sprite
 * whose size grows with the timer after tick 10 (capped at 0x7D00). */
void func_80193150(void *object, SceneGlowModelTimer *timer, SceneGlowModelState *state)
{
    RoomSpriteMatrix matrix;
    SceneGlowModelColor color;
    RoomFxVec4 scale;
    RoomFxVec4 source;
    SceneGlowModelOwner *owner;
    SceneGlowModelState *model;
    int size;

    model = state;
    owner = func_800C2B50();
    func_800794C4(&model->seed, &matrix);
    func_80071A44(&source, 0, 0x10);
    source.x = model->scaleX;
    source.y = model->scaleY;
    source.z = model->scaleZ;
    scale = source;
    func_80078CC4(&matrix, &scale);
    matrix.t[0] = D_8019956C;
    matrix.t[1] = D_800942EC.value + model->height;
    matrix.t[2] = D_8019957C;
    func_800C6D5C(D_80199528, 0, 0);
    if (owner->mode == 0) {
        int tpage = (u16)func_80077A64(0, 1, 0x340, 0x100);
        func_800C6EC0(tpage, (u16)func_80077AA4(0, 0x1D7));
    }
    if (owner->mode == 1) {
        int tpage = (u16)func_80077A64(0, 1, 0x340, 0x160);
        func_800C6EC0(tpage, (u16)func_80077AA4(0, 0x1DB));
    }
    func_800C3134(D_80198860, timer->ticks, &color);
    func_800C6ED8(1);
    func_800C6EF8(D_80199528);
    func_800C6FA0(D_80199528, model->depth);
    func_800C71E4(D_80199528, &matrix);
    func_800C6F4C(D_80199528);
    if (timer->ticks > 10) {
        size = (timer->ticks - 10) << 9;
        if (size > 0x7D00)
            size = 0x7D00;
        func_800C2EAC(3);
        func_800C2FF0(0x20, 0x20);
        func_800C3098(0x100);
        func_800C3238(2);
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
        source.x = size;
        source.y = size;
        source.z = size;
        func_80078CC4(&matrix, &source);
        matrix.t[0] = D_8019956C;
        matrix.t[1] = 0;
        matrix.t[2] = D_8019957C;
        func_800C42A4(D_801996B0, &matrix, 1);
    }
}
