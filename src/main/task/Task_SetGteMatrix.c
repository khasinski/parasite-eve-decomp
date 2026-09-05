#include "common.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define S32_AT(ptr, off) (*(s32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

extern u8 *D2F0_setup[] __asm__("D_8009D2F0");
extern u8 *D2F0_matrix[] __asm__("D_8009D2F0");
extern u8 *D2F0_axis1[] __asm__("D_8009D2F0");
extern u8 *D2F0_axis2[] __asm__("D_8009D2F0");
extern u8 *D2F0_translation[] __asm__("D_8009D2F0");
extern u8 *D2F0_color0[] __asm__("D_8009D2F0");
extern u8 *D2F0_color1[] __asm__("D_8009D2F0");
extern u8 *D2F0_color2[] __asm__("D_8009D2F0");
extern u8 *D2F0_render0[] __asm__("D_8009D2F0");
extern u8 *D2F0_render1[] __asm__("D_8009D2F0");
extern u8 *D2F0_render2[] __asm__("D_8009D2F0");
extern u8 *D2F0_render3[] __asm__("D_8009D2F0");
extern u8 *D2F0_render4[] __asm__("D_8009D2F0");
extern u8 *D2F0_render5[] __asm__("D_8009D2F0");
extern u8 *D2F0_flags[] __asm__("D_8009D2F0");
extern u8 *D2F0_redraw[] __asm__("D_8009D2F0");
typedef struct GlobalIntSlot {
    int value;
    u8 pad[8];
} GlobalIntSlot;

extern GlobalIntSlot CDDC_draw0 __asm__("D_8009CDDC");
extern GlobalIntSlot CDDC_toggle0_load __asm__("D_8009CDDC");
extern GlobalIntSlot CDDC_toggle0_store __asm__("D_8009CDDC");
extern GlobalIntSlot CDDC_draw1 __asm__("D_8009CDDC");
extern GlobalIntSlot CDDC_toggle1_load __asm__("D_8009CDDC");
extern GlobalIntSlot CDDC_toggle1_store __asm__("D_8009CDDC");
extern int *D_8009CE00;
extern u8 *D_8009D300;
extern u32 D_800B89F8[];
extern u32 D_800BEA40[];

void RotMatrix(GteRotation *rotation, GteMatrix *matrix);
void Render_InitRoomPrimState(u8 *object);
void Anim_BuildRotationMatrices(u8 *object, u8 *animation, int frame, int mode);
void Render_TransformVertices(u8 *object);
void Render_TransformSkinnedVertices(u8 *object, u32 *view_matrix);
void Render_DrawObject(u8 *object, u32 *prim_state);
void Render_UpdateClutTable(u8 *object, int force, int buffer_index);

int Task_SetGteMatrix(int **args) {
    u8 *setup_actor;
    u8 *matrix_actor;
    u8 *color_actor;
    u8 *render_actor;
    u8 *flag_actor;
    GteMatrix scale_matrix;
    GteMatrix *first_matrix;
    GteMatrix *second_matrix;
    GteMatrix *third_matrix;
    GteMatrix *translation_matrix;
    int first_draw_slot;
    int second_draw_slot;
    int result;
    int *script_ptr;
    u8 *task_state;
    u32 flags;

    setup_actor = D2F0_setup[0];
    S32_AT(setup_actor, 0x1FC) = S16_AT(setup_actor, 0x2A);
    S32_AT(setup_actor, 0x200) = S16_AT(setup_actor, 0x2E);
    S32_AT(setup_actor, 0x204) = S16_AT(setup_actor, 0x32);
    U16_AT(setup_actor, 0x1E0) = U16_AT(setup_actor, 0x38);
    U16_AT(setup_actor, 0x1E2) = U16_AT(setup_actor, 0x3A);
    U16_AT(setup_actor, 0x1E4) = U16_AT(setup_actor, 0x3C);
    RotMatrix((GteRotation *)(setup_actor + 0x1E0),
              (GteMatrix *)(setup_actor + 0x1E8));

    matrix_actor = D2F0_matrix[0];
    scale_matrix.m[0][0] = U16_AT(matrix_actor, 0x26);
    scale_matrix.m[1][1] = U16_AT(matrix_actor, 0x26);
    scale_matrix.m[2][2] = U16_AT(matrix_actor, 0x26);
    first_matrix = (GteMatrix *)(matrix_actor + 0x1E8);
    scale_matrix.t[2] = 0;
    scale_matrix.t[1] = 0;
    scale_matrix.t[0] = 0;
    scale_matrix.m[2][1] = 0;
    scale_matrix.m[2][0] = 0;
    scale_matrix.m[1][2] = 0;
    scale_matrix.m[1][0] = 0;
    scale_matrix.m[0][2] = 0;
    scale_matrix.m[0][1] = 0;

    gte_ldrotmatrix(first_matrix);

    gte_ldrtir12_matrix_column(&scale_matrix.m[0][0]);
    gte_stir123_matrix_column(&first_matrix->m[0][0]);

    gte_ldrtir12_matrix_column(&scale_matrix.m[0][1]);
    second_matrix = (GteMatrix *)(D2F0_axis1[0] + 0x1E8);
    gte_stir123_matrix_column(&second_matrix->m[0][1]);

    gte_ldrtir12_matrix_column(&scale_matrix.m[0][2]);
    third_matrix = (GteMatrix *)(D2F0_axis2[0] + 0x1E8);
    gte_stir123_matrix_column(&third_matrix->m[0][2]);

    translation_matrix = (GteMatrix *)(D2F0_translation[0] + 0x1E8);
    gte_ldtransmatrix(translation_matrix);
    gte_ldv0_word3_at(scale_matrix.t);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtv0tr_sf0();
    gte_swc2_9_0(translation_matrix->t);
    gte_swc2_10_4(translation_matrix->t);
    gte_swc2_11_8(translation_matrix->t);

    color_actor = D2F0_color0[0];
    U8_AT(color_actor, 0x23C) = *args[0];
    U8_AT(D2F0_color1[0], 0x23D) = *args[1];
    U8_AT(D2F0_color2[0], 0x23E) = *args[2];

    Render_InitRoomPrimState(D2F0_render0[0] + 0x1B4);
    render_actor = D2F0_render1[0];
    Anim_BuildRotationMatrices(render_actor + 0x1B4,
                               PTR_AT(render_actor, 0x1B0), 0, 1);
    Render_TransformVertices(D2F0_render2[0] + 0x1B4);
    Render_TransformSkinnedVertices(D2F0_render3[0] + 0x1B4, D_800B89F8);
    Render_DrawObject(D2F0_render4[0] + 0x1B4, D_800BEA40);
    Render_UpdateClutTable(D2F0_render5[0] + 0x1B4, 1,
                           (s16)CDDC_draw0.value);

    flags = U32_AT(flag_actor = D2F0_flags[0], 0x98);
    if ((flags & 0x10000000) != 0) {
        first_draw_slot = CDDC_toggle0_load.value;
        first_draw_slot ^= 1;
        CDDC_toggle0_store.value = first_draw_slot;
        Render_DrawObject(flag_actor + 0x1B4, D_800BEA40);
        Render_UpdateClutTable(D2F0_redraw[0] + 0x1B4, 1,
                               (s16)CDDC_draw1.value);
        second_draw_slot = CDDC_toggle1_load.value;
        second_draw_slot ^= 1;
        CDDC_toggle1_store.value = second_draw_slot;
        goto success;
    }
    if ((flags & 0x08000000) == 0) {
        result = 0;
        script_ptr = D_8009CE00;
        U32_AT(flag_actor, 0x98) = flags | 0x08000000;
        task_state = D_8009D300;
        D_8009CE00 = script_ptr - 5;
        S32_AT(task_state, 0x10) = 1;
        return result;
    }
    U32_AT(flag_actor, 0x98) = flags & 0xF7FFFFFF;
success:
    return 1;
}
