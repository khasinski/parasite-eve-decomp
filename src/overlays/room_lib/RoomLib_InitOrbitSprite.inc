#include "common.h"
#include "pe1/room_fx.h"

typedef struct RoomOrbitInitView {
    int pad0;
    unsigned char transform[0x14];
    int baseX;
    int baseY;
    int baseZ;
} RoomOrbitInitView;

#ifdef ROOMLIB_INIT_ORBIT_SPRITE_COMPACT_SEEDS
#define ROOMLIB_ORBIT_SEED0 D_8018EFF4
#define ROOMLIB_ORBIT_SEED1 D_8018EFFC
#define ROOMLIB_ORBIT_SEED2 D_8018F004
#define ROOMLIB_ORBIT_SEED3 D_8018F00C
#else
#define ROOMLIB_ORBIT_SEED0 D_8018F014
#define ROOMLIB_ORBIT_SEED1 D_8018F01C
#define ROOMLIB_ORBIT_SEED2 D_8018F024
#define ROOMLIB_ORBIT_SEED3 D_8018F02C
#endif

extern RoomFxSeed8 ROOMLIB_ORBIT_SEED0;
extern RoomFxSeed8 ROOMLIB_ORBIT_SEED1;
extern RoomFxSeed8 ROOMLIB_ORBIT_SEED2;
extern RoomFxSeed8 ROOMLIB_ORBIT_SEED3;
extern short D_800942EC;

RoomOrbitInitView *func_800C2B50(void);
int *func_800C2B10(int index);
void func_80078C34(
    void *transform, RoomFxSeed8 *seed, unsigned short *output);

void ROOMLIB_INIT_ORBIT_SPRITE_NAME(
    void *unused0, void *unused1, RoomOrbitSpriteFxParams *state) {
    RoomFxSeed8 seed0;
    RoomFxSeed8 seed1;
    RoomFxSeed8 seed2;
    RoomFxSeed8 seed3;
    unsigned short position[4];
    RoomOrbitInitView *view;

    view = func_800C2B50();
    seed0 = ROOMLIB_ORBIT_SEED0;
    seed1 = ROOMLIB_ORBIT_SEED1;
    seed2 = ROOMLIB_ORBIT_SEED2;
    seed3 = ROOMLIB_ORBIT_SEED3;

    if (*func_800C2B10(1) == 0) {
        func_80078C34(&view->transform, &seed0,
                      (unsigned short *)&state->pad6[2]);
        func_80078C34(&view->transform, &seed1, position);
    } else {
        func_80078C34(&view->transform, &seed2,
                      (unsigned short *)&state->pad6[2]);
        func_80078C34(&view->transform, &seed3, position);
    }

    state->x = position[0] + view->baseX;
    PE1_COMPILER_MEMORY_BARRIER();
    state->y = D_800942EC;
    state->z = position[2] + view->baseZ;
    state->depth = 0x80;
    state->scale = 0x800;
    *(unsigned short *)state->pad14 = 0x258;
    state->alpha = 0;
}
