#include "common.h"
#include "pe1/room_fx.h"

#ifdef ROOMLIB_DRAW_ORBIT_BURST_COMPACT_GLOBALS
#define ROOMLIB_ORBIT_BURST_CONFIG_A D_801900A4
#define ROOMLIB_ORBIT_BURST_CONFIG_B D_801900B4
#else
#define ROOMLIB_ORBIT_BURST_CONFIG_A D_80194394
#define ROOMLIB_ORBIT_BURST_CONFIG_B D_801943A4
#endif

extern unsigned char ROOMLIB_ORBIT_BURST_CONFIG_A;
extern unsigned char ROOMLIB_ORBIT_BURST_CONFIG_B;
extern short D_800942EC;

char *func_800C2B50(void);
void func_800C2EAC(int value);
void func_800C2FF0(int x, int y);
void func_800C3098(int value);
void func_800C3238(int value);
void func_800794C4(RoomOrbitBurstVector *vector, RoomSpriteMatrix *matrix);
void func_80071A44(RoomFxVec4 *vector, int value, int shift);
void func_80078CC4(RoomSpriteMatrix *matrix, RoomFxVec4 *scale);
void func_800C42A4(void *config, RoomSpriteMatrix *matrix, int mode);

void ROOMLIB_DRAW_ORBIT_BURST_NAME(
    void *unused0, void *unused1, RoomOrbitBurstState *state) {
    RoomSpriteMatrix matrix;
    RoomFxVec4 scratchScale;
    RoomFxVec4 scale;
    char *owner;
    RoomOrbitBurstVector *position;
    unsigned char *configA;
    unsigned char *configB;
    unsigned int i;

    owner = func_800C2B50();
    PE1_COMPILER_USE(owner);
    i = 0;
    configA = &ROOMLIB_ORBIT_BURST_CONFIG_A;
    configB = &ROOMLIB_ORBIT_BURST_CONFIG_B;
    PE1_COMPILER_USE(configB);
    position = state->position;
    func_800C2EAC(owner[0x24]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

    while (i < 8) {
        if (state->active[i] == 1) {
            func_800794C4(&state->secondary[i], &matrix);
            func_80071A44(&scale, 0, 0x10);
            scale.x = state->scale;
            scale.y = state->scale;
            scale.z = 0x400;
            scratchScale = scale;
            func_80078CC4(&matrix, &scratchScale);

            matrix.t[0] = position->x;
            matrix.t[1] = position->y;
            matrix.t[2] = position->z;
            *configA = state->frame[i] * 2 + 0x20;
            func_800C42A4(configA - 4, &matrix, 1);

            matrix.t[0] = position->x;
            matrix.t[1] = D_800942EC;
            matrix.t[2] = position->z;
            *configB = state->frame[i] * 2 + 0x20;
            func_800C42A4(configB - 4, &matrix, 0);
        }
        i++;
        position++;
    }
}
