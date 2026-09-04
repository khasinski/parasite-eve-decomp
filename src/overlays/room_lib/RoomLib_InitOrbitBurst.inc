#include "common.h"
#include "pe1/room_fx.h"

typedef struct RoomOrbitBurstView {
    int pad0;
    unsigned char transform[0x14];
    int baseX;
    int baseY;
    int baseZ;
} RoomOrbitBurstView;

extern short D_800942EC;

RoomOrbitBurstView *func_800C2B50(void);
int func_80071A54(void);

void ROOMLIB_INIT_ORBIT_BURST_NAME(
    void *unused0, void *unused1, RoomOrbitBurstState *state) {
    RoomOrbitBurstView *view;
    short *particle;
    unsigned int i;

    view = func_800C2B50();
    i = 0;
    particle = (short *)state;
    while (i < 8) {
        particle[0] = view->baseX;
        particle[1] = D_800942EC;
        particle[2] = view->baseZ;
        particle[0x20] = func_80071A54() % 0x2000 - 0x1000;
        particle[0x22] = func_80071A54() % 0x2000 - 0x1000;
        particle[0x21] = -(func_80071A54() % 8000 + 8000);
        particle[0x40] = 0;
        particle[0x41] = 0;
        particle[0x42] = func_80071A54() % 0x1000;
        state->active[i] = 1;
        state->frame[i] = func_80071A54() % 3;
        i++;
        particle += 4;
    }

    state->depth = 0x80;
    state->scale = 0x200;
    state->phaseStep = 0xC8;
    state->count = 8;
}
