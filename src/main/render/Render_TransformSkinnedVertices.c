/* Matching debt: register pins, empty compiler barriers, physical scratchpad
 * accesses and native -G8 compiler/assembler addressing. GTE operations are
 * individually wrapped in the shared header. No CPU instruction ASM. */
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_object.h"

#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))

#define Render_SkinnedLoadRotMatrix(matrix)                                                        \
    {                                                                                              \
        register int w0 asm("$12");                                                                \
        register int w1 asm("$13");                                                                \
        register int w2 asm("$12");                                                                \
        register int w3 asm("$13");                                                                \
        register int w4 asm("$14");                                                                \
                                                                                                   \
        w0 = (matrix)[0];                                                                          \
        w1 = (matrix)[1];                                                                          \
                                                                                                   \
        gte_ctc2_0(w0);                                                                            \
        gte_ctc2_1(w1);                                                                            \
        w2 = (matrix)[2];                                                                          \
        w3 = (matrix)[3];                                                                          \
        w4 = (matrix)[4];                                                                          \
        gte_ctc2_2(w2);                                                                            \
        gte_ctc2_3(w3);                                                                            \
        gte_ctc2_4(w4);                                                                            \
    }

#define Render_SkinnedLoadTrans(matrix)                                                            \
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

#define Render_SkinnedLoadFullMatrix(matrix)                                                       \
    {                                                                                              \
        Render_SkinnedLoadRotMatrix(matrix);                                                       \
        Render_SkinnedLoadTrans(matrix);                                                           \
    }

#define Render_SkinnedTransformVec(src, dst)                                                       \
    {                                                                                              \
        register int x asm("$12");                                                                 \
        register int y asm("$13");                                                                 \
        register int z asm("$14");                                                                 \
                                                                                                   \
        gte_lwc2_0_0(src);                                                                         \
        gte_lwc2_1_4(src);                                                                         \
        gte_cop2_hazard_slot();                                                                    \
        gte_cop2_hazard_slot();                                                                    \
        gte_mvmva_rotation_v0_translation_sf12();                                                  \
        {                                                                                          \
            u8 *output = (u8 *)(dst);                                           \
            asm("" : "=r"(output) : "0"(output));                                                  \
            gte_mfc2_9(x);                                                                         \
            gte_mfc2_10(y);                                                                        \
            gte_mfc2_11(z);                                                                        \
            S16_AT(output, 0) = x;                                                                 \
            S16_AT(output, 2) = y;                                                                 \
            S16_AT(output, 4) = z;                                                                 \
        }                                                                                          \
    }

#define Skinned_LoadAxis(src)                                                                      \
    {                                                                                              \
        register int x asm("$12"), y asm("$13"), z asm("$14");                                     \
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
#define Skinned_StoreAxis(dst)                                                                     \
    {                                                                                              \
        register int x asm("$12"), y asm("$13"), z asm("$14");                                     \
        gte_mfc2_9(x);                                                                             \
        gte_mfc2_10(y);                                                                            \
        gte_mfc2_11(z);                                                                            \
        (dst)[0] = x;                                                                              \
        (dst)[3] = y;                                                                              \
        (dst)[6] = z;                                                                              \
    }
#define Skinned_RootAxis(actor)                                                                    \
    {                                                                                              \
        register u16 *src asm("$9") = (u16 *)(u8 *)actor->matrices;                                \
        \
        Skinned_LoadAxis(src);                                                                     \
    }
#define Skinned_SelectedAxis(actor)                                                                \
    {                                                                                              \
        int index = (s16)actor->table_index;                                    \
        u16 *src;                                                               \
        \
        src = (u16 *)((u8 *)actor->matrices + index * 32);                                         \
        \
        Skinned_LoadAxis(src);                                                                     \
    }
#define Render_SkinnedBuildMatrix(view_matrix, bone_expr, out_matrix, first_axis)                  \
    {                                                                                              \
        Render_SkinnedLoadRotMatrix(view_matrix);                                                  \
        first_axis;                                                                                \
        Skinned_StoreAxis((s16 *)(out_matrix));                                                    \
        {                                                                                          \
            u16 *src = (u16 *)(bone_expr) + 1;                                  \
            asm("" : "=r"(src) : "0"(src));                                                        \
            Skinned_LoadAxis(src);                                                                 \
            {                                                                                      \
                register s16 *dst asm("$9") = (s16 *)0x1F800002;                                   \
                asm("" : "=r"(dst) : "0"(dst));                                                    \
                Skinned_StoreAxis(dst);                                                            \
            }                                                                                      \
        }                                                                                          \
        {                                                                                          \
            u16 *src = (u16 *)(bone_expr) + 2;                                  \
            asm("" : "=r"(src) : "0"(src));                                                        \
            Skinned_LoadAxis(src);                                                                 \
            {                                                                                      \
                register s16 *dst asm("$9") = (s16 *)0x1F800004;                                   \
                asm("" : "=r"(dst) : "0"(dst));                                                    \
                Skinned_StoreAxis(dst);                                                            \
            }                                                                                      \
        }                                                                                          \
        Render_SkinnedLoadTrans(view_matrix);                                                      \
        {                                                                                          \
            u8 *src = (u8 *)(bone_expr) + 20;                                   \
            register u32 xy asm("$12"), y asm("$13");                                              \
            asm("" : "=r"(src) : "0"(src));                                                        \
            y = U16_AT(src, 4);                                                                    \
            xy = U16_AT(src, 0);                                                                   \
            y <<= 16;                                                                              \
            xy |= y;                                                                               \
            gte_mtc2_0(xy);                                                                        \
            gte_lwc2_1_8(src);                                                                     \
            gte_cop2_hazard_slot();                                                                \
            gte_cop2_hazard_slot();                                                                \
            gte_mvmva_rotation_v0_translation_sf12();                                              \
            {                                                                                      \
                register s32 *dst asm("$9") = (s32 *)0x1F800014;                                   \
                \
                gte_swc2_25_0(dst);                                                                \
                gte_swc2_26_4(dst);                                                                \
                gte_swc2_27_8(dst);                                                                \
            }                                                                                      \
        }                                                                                          \
    }
#define Render_SkinnedProject(src, out)                                                            \
    {                                                                                              \
        gte_lwc2_0_0(src);                                                                         \
        gte_lwc2_1_4(src);                                                                         \
        gte_cop2_hazard_slot();                                                                    \
        gte_cop2_hazard_slot();                                                                    \
        gte_rtps_command();                                                                        \
        {                                                                                          \
            u8 *output = (u8 *)(out);                                           \
            \
            gte_stsxy2(output);                                                                    \
        }                                                                                          \
    }

void Render_TransformSkinnedVertices(RenderObjectEntity *input, u32 *view_input) {
    RenderObjectEntity *actor = input;
    s32 *view_matrix = (s32 *)view_input;
    volatile u16 *scratch_vec = (volatile u16 *)0x1F800020;
    s32 *scratch_matrix = (s32 *)0x1F800000;
    RenderObjectHeader *header;
    asm(""
        : "=r"(actor), "=r"(view_matrix), "=r"(scratch_vec)
        : "0"(actor), "1"(view_matrix), "2"(scratch_vec));
    header = actor->header;
    if (!header) {
        Render_SetupBoneTransforms(actor, view_matrix);
        return;
    }
    {
        int index = header->anchor_matrix_index;
        s32 *matrix;
        matrix = (s32 *)((u8 *)actor->matrices + index * 32);
        Render_SkinnedLoadFullMatrix(matrix);
    }
    asm("" : "=r"(scratch_matrix) : "0"(scratch_matrix));
    {
        int value = header->anchor_y;
        D_8009CD9A = value;
    }
    {
        register u8 *vector asm("$9") = D_8009CD98;
        Render_SkinnedTransformVec(vector, &actor->anchor_position);
    }
    {
        register s32 *matrix asm("$9") = (s32 *)(u8 *)actor->matrices;
        Render_SkinnedLoadFullMatrix(matrix);
    }
    scratch_vec[0] = U16_AT(actor->model_section14, 0) + actor->animation_value74;
    *(u16 *)0x1F800022 = U16_AT(actor->model_section14, 2) + actor->animation_value76;
    asm volatile("" : : : "memory");
    *(u16 *)0x1F800024 = U16_AT(actor->model_section14, 4) + actor->animation_value78;
    Render_SkinnedTransformVec(scratch_vec, &actor->animation_value74);
    {
        int index = (s16)actor->table_index;
        s32 *matrix = (s32 *)((u8 *)actor->matrices + index * 32);
        u8 *point;
        Render_SkinnedLoadFullMatrix(matrix);
        point = (u8 *)actor->bounds_vertices + index * 16;
        Render_SkinnedTransformVec(point, &actor->target_x);
    }
    Render_SkinnedBuildMatrix(view_matrix, (s32 *)(u8 *)actor->matrices, scratch_matrix,
                              Skinned_RootAxis(actor));
    Render_SkinnedLoadFullMatrix(scratch_matrix);
    {
        register u8 *point asm("$9") = (u8 *)actor->projection_origin;
        Render_SkinnedProject(point, &actor->projected_x);
    }
    Render_SkinnedBuildMatrix(view_matrix,
                              (s32 *)((u8 *)actor->matrices + (s16)actor->table_index * 32),
                              scratch_matrix, Skinned_SelectedAxis(actor));
    Render_SkinnedLoadFullMatrix(scratch_matrix);
    {
        int index = (s16)actor->table_index;
        u8 *point;
        point = (u8 *)actor->bounds_vertices + index * 16;
        Render_SkinnedProject(point, &actor->projected_target_x);
    }
}
