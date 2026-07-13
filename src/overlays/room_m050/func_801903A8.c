typedef short s16;
typedef int s32;

typedef struct Overlay024Variant38State {
    char pad0[0x22];
    s16 field22;
    s16 resource_selector;
    s16 active_flag;
    s16 height;
    s16 width;
    s16 field2C;
    s16 transform_index;
    s32 field30;
} Overlay024Variant38State;

s32 *func_800C2B10(s32 index);

void func_801903A8(void *arg0, void *arg1, Overlay024Variant38State *state) {
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
}
