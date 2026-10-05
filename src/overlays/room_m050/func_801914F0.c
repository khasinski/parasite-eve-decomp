#include "common.h"
#include "../room_lib/RoomLib_Overlay024.h"
s32 *func_800C2B10(s32 index);
s32 func_80071A54(void);

void func_801914F0(void *arg0, void *arg1, RoomOverlay024Variant290SetupState *state) {
    s32 random;
    s32 height;

    state->transform_index = *func_800C2B10(1);
    state->resource_selector = *func_800C2B10(2);
    state->active_flag = 1;

    if (state->resource_selector == -1) {
        height = 0x290;
        state->width = 0;
    } else {
        state->width = 0x80;
        height = 0x290;
    }
    state->height = height;

    state->field2C = 0;
    state->field30 = 0;
    state->field22 = 0;
    state->sparkle_timer = 0;
    random = func_80071A54();
    state->random_mod = random % 40;
}
