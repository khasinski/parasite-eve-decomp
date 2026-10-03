/* Matching debt: the scratch-column store pointers are pinned to t1, and raw
 * pointer casts remain. Matrix and column transfers use the stock PSY-Q GTE
 * macros (gte_ldrotmatrix, gte_ldclmv, gte_stclmv, gte_ldlv0, gte_stsv). */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_object.h"
#define BoneLoadRotMatrix(matrix) gte_ldrotmatrix(matrix)
#define BoneLoadTrans(matrix) gte_ldtransmatrix(matrix)

#define BoneLoadFullMatrix(matrix)                                                                 \
    {                                                                                              \
        BoneLoadRotMatrix(matrix);                                                                 \
        BoneLoadTrans(matrix);                                                                     \
    }

#define Bone_LoadAxis(src)                                                                         \
    {                                                                                              \
        gte_ldclmv(src);                                                                           \
        gte_rtir();                                                                                \
    }
#define Bone_StoreAxis(dst) gte_stclmv(dst)

#define Bone_StoreVec(output_expr)                                                                 \
    {                                                                                              \
        s16 *out = (s16 *)(output_expr);                                                           \
        gte_stsv(out);                                                                             \
    }
void Render_SetupBoneTransforms(RenderObjectEntity *input, s32 *view_input) {
    s32 *scratch = (s32 *)0x1F800000;
    RenderObjectEntity *actor = input;
    s32 *view = view_input;
    RenderObjectEntity *source;
    int index;
    s32 *matrix;
    source = actor->animation_source;
    index = (s16)actor->table_index;
    matrix = (s32 *)source->matrices;
    {
        int offset = index * 32;

        matrix = (s32 *)((u8 *)matrix + offset);
    }
    BoneLoadFullMatrix(matrix);
    {
        u8 *point = (u8 *)source->bounds_vertices;
        index *= 16;
        point += index;

        gte_lwc2_0_0(point);
        gte_lwc2_1_4(point);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_v0_translation_sf12();
    }
    /* Headerless objects reuse the first override XYZ as a position output. */
    Bone_StoreVec(&actor->rotation_overrides[0].x);
    Bone_StoreVec(&actor->anchor_position);
    Bone_StoreVec(&actor->target_x);
    BoneLoadRotMatrix(view);
    Bone_LoadAxis((u16 *)matrix);
    Bone_StoreAxis((s16 *)scratch);
    {
        u16 *src = (u16 *)matrix + 1;
        Bone_LoadAxis(src);
        {
            register s16 *dst asm("$9") = (s16 *)0x1F800002;
            Bone_StoreAxis(dst);
        }
    }
    {
        u16 *src = (u16 *)matrix + 2;
        Bone_LoadAxis(src);
        {
            register s16 *dst asm("$9") = (s16 *)0x1F800004;
            Bone_StoreAxis(dst);
        }
    }
    BoneLoadTrans(view);
    matrix += 5;

    {
        gte_ldlv0(matrix);
        gte_rt();
        {
            register s32 *dst asm("$9") = (s32 *)0x1F800014;

            gte_swc2_25_0(dst);
            gte_swc2_26_4(dst);
            gte_swc2_27_8(dst);
        }
    }
    BoneLoadFullMatrix(scratch);
    /* In this mode the halfwords at +0x2C form the projection input vector. */
    {
        u16 *point = &actor->table_value2c;

        gte_lwc2_0_0(point);
        gte_lwc2_1_4(point);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_rtps_command();
        actor->animation_value7c = source->animation_value7c;
        {
            s16 *out = &actor->projected_target_x;
            gte_stsxy2(out);
        }
        actor->table_value2c += actor->table_value70;
        gte_lwc2_0_0(point);
        gte_lwc2_1_4(point);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_rtps_command();
        actor->animation_value74 = source->animation_value74;
        actor->animation_value76 = source->animation_value76;
        actor->animation_value78 = source->animation_value78;
        {
            s16 *out = &actor->projected_x;
            gte_stsxy2(out);
        }
        actor->table_value2c -= actor->table_value70;
    }
}
