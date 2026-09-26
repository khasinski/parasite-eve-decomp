/* Matching debt: register pins, empty compiler barriers and raw pointer
 * casts. GTE operations and authorized hazard nops use individual macros. */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_object.h"
#define BoneLoadRotMatrix(matrix)                                                                  \
    {                                                                                              \
        register s32 x asm("$12");                                                                 \
        register s32 y asm("$13");                                                                 \
        register s32 z asm("$14");                                                                 \
        x = (matrix)[0];                                                                           \
        y = (matrix)[1];                                                                           \
        gte_ctc2_0(x);                                                                             \
        gte_ctc2_1(y);                                                                             \
        x = (matrix)[2];                                                                           \
        y = (matrix)[3];                                                                           \
        z = (matrix)[4];                                                                           \
        gte_ctc2_2(x);                                                                             \
        gte_ctc2_3(y);                                                                             \
        gte_ctc2_4(z);                                                                             \
    }

#define BoneLoadTrans(matrix)                                                                      \
    {                                                                                              \
        register int w5 asm("$12");                                                                \
        register int w6 asm("$13");                                                                \
        register int w7 asm("$14");                                                                \
                                                                                                   \
        w5 = (matrix)[5];                                                                          \
        w6 = (matrix)[6];                                                                          \
                                                                                                   \
        gte_ctc2_5(w5);                                                                            \
        w7 = (matrix)[7];                                                                          \
        gte_ctc2_6(w6);                                                                            \
        gte_ctc2_7(w7);                                                                            \
    }

#define BoneLoadFullMatrix(matrix)                                                                 \
    {                                                                                              \
        BoneLoadRotMatrix(matrix);                                                                 \
        BoneLoadTrans(matrix);                                                                     \
    }

#define Bone_LoadAxis(src)                                                                         \
    {                                                                                              \
        register int x asm("$12");                                                                 \
        register int y asm("$13");                                                                 \
        register int z asm("$14");                                                                 \
        x = (src)[0];                                                                              \
        y = (src)[3];                                                                              \
        z = (src)[6];                                                                              \
        gte_mtc2_9(x);                                                                             \
        gte_mtc2_10(y);                                                                            \
        gte_mtc2_11(z);                                                                            \
        gte_cop2_hazard_slot();                                                                    \
        gte_cop2_hazard_slot();                                                                    \
        gte_mvmva_rotation_ir_sf12();                                                              \
    }
#define Bone_StoreAxis(dst)                                                                        \
    {                                                                                              \
        register int x asm("$12");                                                                 \
        register int y asm("$13");                                                                 \
        register int z asm("$14");                                                                 \
        gte_mfc2_9(x);                                                                             \
        gte_mfc2_10(y);                                                                            \
        gte_mfc2_11(z);                                                                            \
        (dst)[0] = x;                                                                              \
        (dst)[3] = y;                                                                              \
        (dst)[6] = z;                                                                              \
    }

#define Bone_StoreVec(output_expr)                                                                 \
    {                                                                                              \
        register s16 *out asm("$2") = (s16 *)(output_expr);                                        \
        register int x asm("$12");                                                                 \
        register int y asm("$13");                                                                 \
        register int z asm("$14");                                                                 \
        asm("" : "=r"(out) : "0"(out));                                                            \
        gte_mfc2_9(x);                                                                             \
        gte_mfc2_10(y);                                                                            \
        gte_mfc2_11(z);                                                                            \
        out[0] = x;                                                                                \
        out[1] = y;                                                                                \
        out[2] = z;                                                                                \
    }
void Render_SetupBoneTransforms(RenderObjectEntity *input, s32 *view_input) {
    s32 *scratch = (s32 *)0x1F800000;
    RenderObjectEntity *actor = input;
    s32 *view = view_input;
    register RenderObjectEntity *source asm("$7");
    int index;
    register s32 *matrix asm("$6");
    asm volatile("" : "=r"(scratch) : "0"(scratch) : "memory");
    source = actor->animation_source;
    asm volatile("" : "=r"(source) : "0"(source) : "memory");
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
    asm volatile("" : : : "memory");
    {
        u16 *src = (u16 *)matrix + 1;
        asm("" : "=r"(src) : "0"(src));
        Bone_LoadAxis(src);
        {
            register s16 *dst asm("$9") = (s16 *)0x1F800002;
            asm("" : "=r"(dst) : "0"(dst));
            Bone_StoreAxis(dst);
        }
    }
    asm volatile("" : "=r"(matrix) : "0"(matrix) : "memory");
    {
        u16 *src = (u16 *)matrix + 2;
        asm("" : "=r"(src) : "0"(src));
        Bone_LoadAxis(src);
        {
            register s16 *dst asm("$9") = (s16 *)0x1F800004;
            asm("" : "=r"(dst) : "0"(dst));
            Bone_StoreAxis(dst);
        }
    }
    BoneLoadTrans(view);
    matrix += 5;

    {
        register u32 xy asm("$12");
        register u32 y asm("$13");
        y = *(u16 *)((u8 *)matrix + 4);
        xy = *(u16 *)matrix;
        y <<= 16;
        xy |= y;
        gte_mtc2_0(xy);
        gte_lwc2_1_8(matrix);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_v0_translation_sf12();
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
        asm volatile("" : "=r"(actor) : "0"(actor) : "memory");
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
        asm volatile("" : "=r"(actor) : "0"(actor) : "memory");
        {
            s16 *out = &actor->projected_x;
            gte_stsxy2(out);
        }
        actor->table_value2c -= actor->table_value70;
    }
}
