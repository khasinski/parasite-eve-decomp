#include "common.h"

typedef struct SceneE08RandomOffset {
    s16 x, y, z, phase;
} SceneE08RandomOffset;

typedef struct SceneE08RandomEntries {
    u8 pad00[12];
    u16 counter[10];
    u8 pad20[4];
    u8 active[10];
} SceneE08RandomEntries;

typedef struct SceneE08RandomState {
    u8 pad00;
    u8 status;
    s16 elapsedFrames;
} SceneE08RandomState;

extern int func_80071A54(void);
extern SceneE08RandomOffset D_801994D8[];

void func_801938FC(int unused, SceneE08RandomState *state, SceneE08RandomEntries *entries) {
    unsigned int i;
    for (i = 0; i < 10; ++i) {
        if (entries->active[i] == 1) {
            ++entries->counter[i];
            D_801994D8[i].x = func_80071A54() % 160 - 80;
            D_801994D8[i].y = func_80071A54() % 160 - 80;
            D_801994D8[i].z = func_80071A54() % 160 - 80;
            D_801994D8[i].phase = func_80071A54() % 512 - 256;
        } else if (func_80071A54() % 20 == 0) {
            entries->active[i] = 1;
        }
    }
    if (state->elapsedFrames >= 61) {
        state->status = 2;
    }
}
