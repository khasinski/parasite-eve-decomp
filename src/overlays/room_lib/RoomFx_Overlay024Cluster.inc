#include "common.h"
#include "RoomLib_Overlay024.h"

void func_80078C34(void *transform, RoomOverlay024MatrixSeed8 *seed, void *out);

void func_80192494(void) {
}

extern RoomOverlay024MatrixSeed8 D_8018F01C;
extern RoomOverlay024VectorSeed D_8018F024;
extern void *D_80192C34;
void func_80078CC4(RoomOverlay024ViewMatrix *matrix, RoomOverlay024VectorSeed *seed);
void func_800C2EAC(u8 arg0);
void func_800C2FF0(s32 arg0, s32 arg1);
void func_800C3098(s32 arg0);
void func_800C3238(s32 arg0);
void func_800C6D5C(void *arg0, s32 arg1, s32 arg2);
void func_800C6EE8(s32 arg0);
s32 func_80077A64(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 func_80077AA4(s32 arg0, s32 arg1);
void func_800C6EC0(s32 arg0, s32 arg1);
void func_800C6ED8(s32 arg0);
void func_800C6EF8(void *arg0);
void func_800C6FA0(void *arg0, s32 arg1);
void func_800C71E4(void *arg0, RoomOverlay024ViewMatrix *matrix);
void func_800C6F4C(void *arg0);

void func_8019249C(void) {
    RoomOverlay024Root **root;
    RoomOverlay024View *view;
    RoomOverlay024ViewMatrix matrix;
    RoomOverlay024MatrixSeed8 seed;
    short out[4];
    RoomOverlay024VectorSeed vector_seed;
    s32 handle_a;
    s32 handle_b;

    root = func_800C2B50();
    view = (RoomOverlay024View *)(*root)->view;

    seed = D_8018F01C;
    matrix = view->tail.matrix;
    vector_seed = D_8018F024;

    func_80078CC4(&matrix, &vector_seed);
    func_80078C34(&view->tail.matrix, &seed, out);

    matrix.tx += out[0];
    matrix.ty += out[1];
    matrix.tz += out[2];

    func_800C2EAC(*(u8 *)((char *)root + 0x24));
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
    func_800C6D5C(D_80192C34, 0, 0);
    func_800C6EE8(-10);
    handle_a = func_80077A64(0, 1, 0x340, 0x100);
    handle_a = (u16)handle_a;
    handle_b = func_80077AA4(0x10, 0x1D7);
    func_800C6EC0(handle_a, (u16)handle_b);
    func_800C6ED8(0);
    func_800C6EF8(D_80192C34);
    func_800C6FA0(D_80192C34, 0x80);
    func_800C71E4(D_80192C34, &matrix);
    func_800C6F4C(D_80192C34);
}

void func_80192680(void) {
}

s32 func_80071A54(void);

extern RoomOverlay024MatrixSeed8 D_8018F01C;

void func_80192688(void *arg0, void *arg1, RoomOverlay024Effect *effect) {
    RoomOverlay024View *view;
    RoomOverlay024MatrixSeed8 seed;
    u16 out[4];
    s32 random;

    view = (RoomOverlay024View *)(*(RoomOverlay024Root **)func_800C2B50())->view;
    seed = D_8018F01C;
    func_80078C34(&view->tail.position.transform, &seed, out);

    if (effect->timer == 0) {
        effect->half14 = 0;
        effect->x = out[0] + view->tail.position.base_x;
        effect->y = out[1] + view->tail.position.base_y;
        effect->z = out[2] + view->tail.position.base_z;
        effect->half8 = 0;
        effect->velocity_y = -8;
        effect->halfC = 0;
        random = func_80071A54();
        effect->duration = 0x96;
        effect->timer = random % 80 + 0x50;
    }
}
