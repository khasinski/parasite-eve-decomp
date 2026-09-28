#include "RoomLib_Overlay024.h"
extern RoomOverlay024MatrixSeed8 D_8018EFF4;
extern RoomOverlay024Vec4 D_8018F00C;
extern u8 D_80192C0C;
extern volatile s16 D_80192C12;
extern s16 D_80192C32;
extern s16 D_80192C42;
extern s16 D_800942EC;

extern char D_80192BFC;
extern char D_80192BF0;
void func_80071A44(void *arg0, s32 arg1, s32 arg2);
s32 func_80071A54(void);
void func_80078CC4(RoomOverlay024Matrix *matrix, RoomOverlay024Vec4 *scale);
void func_800794C4(RoomOverlay024MatrixSeed8 *seed, RoomOverlay024Matrix *matrix);
void func_800C2EAC(u8 arg0);
void func_800C2FF0(s32 arg0, s32 arg1);
void func_800C3098(s32 arg0);
void func_800C3134(void *arg0, s32 arg1, void *arg2);
void func_800C3238(s32 arg0);
void func_800C42A4(void *arg0, RoomOverlay024Matrix *matrix, s32 arg2);

void func_80190CB0(void *arg0, RoomOverlay024VariantControl *control, RoomOverlay024Variant38State *state) {
    RoomOverlay024Root **owner_ptr;
    RoomOverlay024Root *owner;
    RoomOverlay024Transform *transform;
    RoomOverlay024Matrix matrix;
    RoomOverlay024MatrixSeed8 base_seed;
    RoomOverlay024Vec4 scratch_scale;
    RoomOverlay024Vec4 draw_scale;
    RoomOverlay024Vec4 second_scale;
    RoomOverlay024Vec4 third_scale;
    s16 *fade_slot;
    u8 *phase_slot;
    char *draw_slot;
    s16 *sparkle_alpha_slot;
    s16 *final_alpha_slot;
    s32 draw_zero;
    volatile RoomOverlay024Transform *ordered_transform;

    owner_ptr = func_800C2B50();
    owner = *owner_ptr;
    transform = &((RoomOverlay024Transform *)owner->view)[state->transform_index];
    base_seed = D_8018EFF4;

    func_800C2EAC(*(u8 *)((char *)owner_ptr + 0x24));
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

    if (state->active_flag == 0) {
        return;
    }

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

    func_80071A44(&draw_scale, 0, 0x10);
    draw_scale.x = state->drift_value;
    draw_scale.y = state->drift_value >> 1;
    draw_scale.z = state->drift_value;
    scratch_scale = draw_scale;
    func_80078CC4(&matrix, &scratch_scale);

    fade_slot = (s16 *)&D_80192C12;
    *fade_slot = state->fade_alpha >> 1;

    matrix.t[0] = transform->x + (func_80071A54() % 20) - 10;
    matrix.t[1] = transform->y;
    matrix.t[2] = transform->z + (func_80071A54() % 20) - 10;

    if (state->resource_selector == -1) {
        func_800C3134(&D_80192BF0, state->field30, (char *)fade_slot - 0xA);
    } else {
        func_800C3134(&D_80192BFC, state->field30, (char *)fade_slot - 0xA);
    }
    phase_slot = &D_80192C0C;
    draw_slot = (char *)phase_slot - 4;
    *phase_slot = state->phase * 2;
    func_800C42A4(draw_slot, &matrix, 1);
    func_800794C4(&base_seed, &matrix);

    func_80071A44(&second_scale, 0, 0x10);
    second_scale.x = state->drift_value * 2;
    second_scale.y = state->drift_value;
    second_scale.z = state->drift_value * 2;
    draw_scale = second_scale;
    func_80078CC4(&matrix, &draw_scale);

    ordered_transform = transform;
    D_80192C12 = state->fade_alpha >> 2;
    matrix.t[0] = ordered_transform->x;
    matrix.t[1] = D_800942EC;
    draw_zero = 0;
    matrix.t[2] = ordered_transform->z;
    *phase_slot = 8;
    func_800C42A4(draw_slot, &matrix, draw_zero);

    if (control->frame >= 0x5B && state->sparkle_timer > 0) {
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

        second_scale = D_8018F00C;
        func_80078CC4(&matrix, &second_scale);

        matrix.t[0] = state->sparkle_x;
        matrix.t[1] = state->sparkle_y;
        matrix.t[2] = state->sparkle_z;
        sparkle_alpha_slot = &D_80192C42;
        *sparkle_alpha_slot = state->sparkle_alpha;
        func_800C42A4((char *)sparkle_alpha_slot - 0xA, &matrix, 0);
    }

    func_800C3238(3);

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

    func_80071A44(&third_scale, 0, 0x10);
    third_scale.x = state->final_scale;
    third_scale.y = state->final_scale;
    third_scale.z = state->final_scale;
    second_scale = third_scale;
    func_80078CC4(&matrix, &second_scale);

    matrix.t[0] = state->final_x;
    matrix.t[1] = state->final_y;
    matrix.t[2] = state->final_z;
    final_alpha_slot = &D_80192C32;
    *final_alpha_slot = state->final_alpha;
    func_800C42A4((char *)final_alpha_slot - 0xA, &matrix, 1);
}
