#include "common.h"
#include "../room_lib/RoomLib_Overlay024.h"
s32 *func_800C2B10(s32 index);
s32 func_80071A54(void);

void func_80190BF8(void *arg0, void *arg1, RoomOverlay024Variant38SetupState *state) {
    s32 random;

    state->transform_index = *func_800C2B10(1);
    state->resource_selector = *func_800C2B10(2);
    state->active_flag = 1;

    if (state->resource_selector == -1) {
        state->width = 0;
        state->height = 0;
    } else {
        state->width = 0x80;
        state->height = 0x38;
    }

    state->field2C = 0;
    state->field30 = 0;
    state->field22 = 0;
    state->sparkle_timer = 0;
    random = func_80071A54();
    state->random_mod = random % 40;
}
