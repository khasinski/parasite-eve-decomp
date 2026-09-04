#include "common.h"
#include "pe1/room_fx.h"

typedef struct RoomOrbitBatchFxParams {
    struct {
        short x;
        short y;
        short z;
        short pad6;
    } positions[8];
    unsigned char pad40[0x40];
    short scale;
    unsigned short depth;
    unsigned char alpha;
} RoomOrbitBatchFxParams;

extern unsigned short ROOMLIB_DRAW_ORBIT_BATCH_DEPTH;

char *func_800C2B50(void);
void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_80071A44(void *arg0, int arg1, int arg2);
void func_80078CC4(RoomSpriteMatrix *matrix, RoomFxVec4 *scale);
void func_800C42A4(void *arg0, RoomSpriteMatrix *matrix, int arg2);

void ROOMLIB_DRAW_ORBIT_BATCH_NAME(
    void *unused0, void *unused1, RoomOrbitBatchFxParams *fx) {
    RoomSpriteMatrix matrix;
    RoomFxVec4 scratchScale;
    RoomFxVec4 scale;
    char *owner;
    unsigned short *depthSlot;
    short *position;
    unsigned int i;

    owner = func_800C2B50();
    PE1_COMPILER_USE(owner);
    i = 0;
    depthSlot = &ROOMLIB_DRAW_ORBIT_BATCH_DEPTH;
    func_800C2EAC(owner[0x24]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
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

    func_80071A44(&scale, 0, 0x10);
    scale.x = fx->scale;
    scale.y = fx->scale;
    scale.z = 0x400;
    scratchScale = scale;
    func_80078CC4(&matrix, &scratchScale);

    position = &fx->positions[0].x;
    while (i < 8) {
        matrix.t[0] = position[0];
        matrix.t[1] = position[1];
        matrix.t[2] = position[2];
        *depthSlot = (short)fx->depth >> 1;
        *((unsigned char *)depthSlot - 6) = fx->alpha << 1;
        func_800C42A4((char *)depthSlot - 0xA, &matrix, 1);
        i++;
        position += 4;
    }
}
