/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
/* Script opcodes: draw the current actor's model through a caller-supplied
 * GTE matrix, the render-mode switch wait that pops the script cursor, and
 * the angle from the current actor to a named actor. Contiguous handlers;
 * the angle opcode reaches its actors through array views, absolute under
 * -G8 as well. */
#include "pe1/render_lighting.h"
#include "pe1/global_slot.h"
#include "common.h"
#include "pe1/task_node.h"
#include "pe1/field_actor.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"

#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define S32_AT(ptr, off) (*(s32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

extern FieldActor *D2F0_setup[] __asm__("D_8009D2F0");
extern FieldActor *D2F0_matrix[] __asm__("D_8009D2F0");
extern FieldActor *D2F0_axis1[] __asm__("D_8009D2F0");
extern FieldActor *D2F0_axis2[] __asm__("D_8009D2F0");
extern FieldActor *D2F0_translation[] __asm__("D_8009D2F0");
extern FieldActor *D2F0_color0[] __asm__("D_8009D2F0");
extern FieldActor *D2F0_color1[] __asm__("D_8009D2F0");
extern FieldActor *D2F0_color2[] __asm__("D_8009D2F0");
extern FieldActor *D2F0_render0[] __asm__("D_8009D2F0");
extern FieldActor *D2F0_render1[] __asm__("D_8009D2F0");
extern FieldActor *D2F0_render2[] __asm__("D_8009D2F0");
extern FieldActor *D2F0_render3[] __asm__("D_8009D2F0");
extern FieldActor *D2F0_render4[] __asm__("D_8009D2F0");
extern FieldActor *D2F0_render5[] __asm__("D_8009D2F0");
extern FieldActor *D2F0_flags[] __asm__("D_8009D2F0");
extern FieldActor *D2F0_redraw[] __asm__("D_8009D2F0");
extern Pe1GlobalSlot CDDC_draw0 __asm__("D_8009CDDC");
extern Pe1GlobalSlot CDDC_toggle0_load __asm__("D_8009CDDC");
extern Pe1GlobalSlot CDDC_toggle0_store __asm__("D_8009CDDC");
extern Pe1GlobalSlot CDDC_draw1 __asm__("D_8009CDDC");
extern Pe1GlobalSlot CDDC_toggle1_load __asm__("D_8009CDDC");
extern Pe1GlobalSlot CDDC_toggle1_store __asm__("D_8009CDDC");
extern int *D_8009CE00;
extern u32 D_800B89F8[];

GteMatrix *RotMatrix(GteShortVector *rotation, GteMatrix *matrix);
int Anim_BuildRotationMatrices(RenderObjectEntity *object, void *action, int frame, int mode);
void Render_TransformVertices(RenderObjectEntity *object);

int Task_SetGteMatrix(int **args) {
    FieldActor *setup_actor;
    FieldActor *matrix_actor;
    FieldActor *color_actor;
    FieldActor *render_actor;
    FieldActor *flag_actor;
    GteMatrix scale_matrix;
    register GteMatrix *first_matrix asm("$3");
    GteMatrix *second_matrix;
    GteMatrix *third_matrix;
    GteMatrix *translation_matrix;
    int first_draw_slot;
    int second_draw_slot;
    int result;
    int *script_ptr;
    TaskNode *task_state;
    u32 flags;

    setup_actor = D2F0_setup[0];
    setup_actor->render_object.model_matrix.translation[0] = Pe1Fixed_Integer(&setup_actor->pos_x);
    setup_actor->render_object.model_matrix.translation[1] = Pe1Fixed_Integer(&setup_actor->pos_y);
    setup_actor->render_object.model_matrix.translation[2] = Pe1Fixed_Integer(&setup_actor->pos_z);
    setup_actor->render_object.table_value2c = (u16)setup_actor->rot_x;
    setup_actor->render_object.table_value2e = (u16)setup_actor->rot_y;
    setup_actor->render_object.table_value30 = (u16)setup_actor->rot_z;
    RotMatrix((GteShortVector *)&setup_actor->render_object.table_value2c,
              (GteMatrix *)&setup_actor->render_object.model_matrix);

    matrix_actor = D2F0_matrix[0];
    scale_matrix.m[0][0] = matrix_actor->move_speed;
    scale_matrix.m[1][1] = matrix_actor->move_speed;
    scale_matrix.m[2][2] = matrix_actor->move_speed;
    first_matrix = (GteMatrix *)&matrix_actor->render_object.model_matrix;
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
    gte_stir123_column(first_matrix);

    gte_ldrtir12_matrix_column(&scale_matrix.m[0][1]);
    second_matrix = (GteMatrix *)&D2F0_axis1[0]->render_object.model_matrix;
    gte_stir123_column_at(&second_matrix->m[0][1]);

    gte_ldrtir12_matrix_column(&scale_matrix.m[0][2]);
    third_matrix = (GteMatrix *)&D2F0_axis2[0]->render_object.model_matrix;
    gte_stir123_column_at(&third_matrix->m[0][2]);

    translation_matrix = (GteMatrix *)&D2F0_translation[0]->render_object.model_matrix;
    gte_ldtransmatrix(translation_matrix);
    gte_ldv0_word3_at(scale_matrix.t);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtv0tr_sf0();
    gte_swc2_9_0(translation_matrix->t);
    gte_swc2_10_4(translation_matrix->t);
    gte_swc2_11_8(translation_matrix->t);

    color_actor = D2F0_color0[0];
    color_actor->render_object.shade = *args[0];
    D2F0_color1[0]->render_object.lightNegativeY = *args[1];
    D2F0_color2[0]->render_object.lightPositiveY = *args[2];

    Render_InitRoomPrimState(&D2F0_render0[0]->render_object);
    render_actor = D2F0_render1[0];
    Anim_BuildRotationMatrices(&render_actor->render_object,
                               render_actor->action_data, 0, 1);
    Render_TransformVertices(&D2F0_render2[0]->render_object);
    Render_TransformSkinnedVertices(&D2F0_render3[0]->render_object, D_800B89F8);
    Render_DrawObject(&D2F0_render4[0]->render_object, D_800BEA40.words);
    Render_UpdateClutTable(&D2F0_render5[0]->render_object, 1,
                           (s16)CDDC_draw0.value.signed_value);

    flags = (flag_actor = D2F0_flags[0])->flags;
    if ((flags & 0x10000000) != 0) {
        first_draw_slot = CDDC_toggle0_load.value.signed_value;
        first_draw_slot ^= 1;
        CDDC_toggle0_store.value.signed_value = first_draw_slot;
        Render_DrawObject(&flag_actor->render_object, D_800BEA40.words);
        Render_UpdateClutTable(&D2F0_redraw[0]->render_object, 1,
                               (s16)CDDC_draw1.value.signed_value);
        second_draw_slot = CDDC_toggle1_load.value.signed_value;
        second_draw_slot ^= 1;
        CDDC_toggle1_store.value.signed_value = second_draw_slot;
        goto success;
    }
    if ((flags & 0x08000000) == 0) {
        result = 0;
        script_ptr = D_8009CE00;
        flag_actor->flags = flags | 0x08000000;
        task_state = D_8009D300;
        D_8009CE00 = script_ptr - 5;
        task_state->active = 1;
        return result;
    }
    flag_actor->flags = flags & 0xF7FFFFFF;
success:
    return 1;
}

extern int g_RenderStateFlags[];
extern int g_SceneDataTable0;
extern int *g_TaskNodePool;

int Boot_CheckModeSwitch(void) {
    int mode = g_RenderStateFlags[0] & 7;

    if (mode == 4) {
        goto ret_one;
    }
    if (mode != 0) {
        goto pop_state;
    }

ret_one:
    return 1;

pop_state:
    {
        int cursor = g_SceneDataTable0;
        int *node = g_TaskNodePool;
        cursor -= 8;
        g_SceneDataTable0 = cursor;
        node[4] = 1;
        return 0;
    }
}

#define NULL ((void *)0)

int ratan2(int arg0, int arg1);

extern FieldActor *g_PlayerEntity[];
#define g_PlayerEntity (g_PlayerEntity[0])
extern FieldActor *g_FieldActorListHead[];
#define g_FieldActorListHead (g_FieldActorListHead[0])
extern FieldActor *g_CurrentEntity[];
#define g_CurrentEntity (g_CurrentEntity[0])

/* Script op: angle from the current entity to the actor named by
 * (args[0], args[1]), relative to the current entity's yaw, into args[2]. */
s32 Task_GetAngleToEntity(s32 *args[]) {
    s32 key;
    s32 key2;
    FieldActor *node;
    s32 angle;
    s32 dx;
    s32 dz;

    key = *args[0];
    if (key == 0) {
        FieldActor *tmp;

        tmp = g_PlayerEntity;
        if (tmp == NULL) {
            goto fail;
        }
        node = tmp;
        goto found;
    } else {
        key2 = key;
        node = g_FieldActorListHead;
        if (node == NULL) {
            goto fail;
        }
loop:
        if ((node->type_id != key2) ||
            (node->sub_id != *args[1]) ||
            (node->flags & 0x10)) {
            node = node->next;
            if (node != NULL) {
                goto loop;
            }
        }
        if (node != NULL) {
            goto found;
        }
    }

fail:
    *args[2] = -1;
    return 1;

found:
    {
        FieldActor *state = g_CurrentEntity;
        dx = state->pos_x - node->pos_x;
        dz = (state->pos_z - node->pos_z) >> 16;
    }
    angle = 0x1400 - ratan2(dz, dx >> 16);
    if (angle >= 0x1001) {
        angle -= 0x1000;
    }
    angle -= (s16)g_CurrentEntity->rot_y;
    if (angle < 0) {
        angle += 0x1000;
    }
    *args[2] = angle;
    return 1;
}
