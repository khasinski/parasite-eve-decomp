/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_object.h"

/* Matching debt: pins, empty barriers, volatile accesses, raw-offset
 * accessors and an artificial two-word local frame reservation. */

#define S8_AT(ptr, off) (*(s8 *)((u8 *)(ptr) + (off)))
#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

#define Render_XformLoadRotMatrix(matrix) gte_ldrotmatrix((matrix))

#define Render_XformLoadTrans(matrix) gte_ldtransmatrix((matrix))

#define Render_XformLoadFullMatrix(matrix)                                                         \
    {                                                                                              \
        Render_XformLoadRotMatrix((matrix));                                                       \
        Render_XformLoadTrans((matrix));                                                           \
    }

#define Render_XformStoreFullMatrix(matrix)                                                        \
    {                                                                                              \
        register s32 x asm("$12"), y asm("$13"), z asm("$14");                                     \
        gte_cfc2_0(x);                                                                             \
        gte_cfc2_1(y);                                                                             \
        (matrix)[0] = x;                                                                           \
        (matrix)[1] = y;                                                                           \
        gte_cfc2_2(x);                                                                             \
        gte_cfc2_3(y);                                                                             \
        gte_cfc2_4(z);                                                                             \
        (matrix)[2] = x;                                                                           \
        (matrix)[3] = y;                                                                           \
        (matrix)[4] = z;                                                                           \
        gte_cfc2_5(x);                                                                             \
        gte_cfc2_6(y);                                                                             \
        gte_cfc2_7(z);                                                                             \
        (matrix)[5] = x;                                                                           \
        (matrix)[6] = y;                                                                           \
        (matrix)[7] = z;                                                                           \
    }

#define Render_XformTransformAxisZ(src, dst)                                                       \
    {                                                                                              \
        Render_XformLoadAxis(src); \
        *zero8 =                                                                                   \
            ((RenderObjectPart *)((RenderObjectEntity *)actor)->parts)[command].translation_z; \
        Render_XformStoreAxis(dst); \
    }

#define Render_XformLoadAxis(src) \
    { \
        gte_ldclmv((src)); \
        gte_rtir(); \
    }

#define Render_XformStoreAxis(dst) \
    { \
        gte_stclmv((dst)); \
        asm volatile("" : : : "memory"); \
    }

#define Render_XformBuildChildMatrix(src, dst, include_translation)                                \
    {                                                                                              \
        Render_XformTransformAxisZ((u16 *)(src), (s16 *)(dst));                                    \
        \
        {                                                                                          \
            u16 *column = (u16 *)(src) + 1;                                     \
            \
            asm volatile("" : "=r"(column) : "0"(column)); \
            Render_XformLoadAxis(column);                                                          \
            column = (u16 *)(dst) + 1;                                                             \
            \
            asm volatile("" : "=r"(column) : "0"(column)); \
            Render_XformStoreAxis((s16 *)column);                                                  \
        }                                                                                          \
        {                                                                                          \
            register u16 *column asm("$2") = (u16 *)(src) + 2;                                     \
            \
            asm volatile("" : "=r"(column) : "0"(column)); \
            Render_XformLoadAxis(column);                                                          \
            column = (u16 *)(dst) + 2;                                                             \
            \
            asm volatile("" : "=r"(column) : "0"(column)); \
            Render_XformStoreAxis((s16 *)column);                                                  \
        }                                                                                          \
        {                                                                                          \
            u16 *packed;                                                        \
            if (S8_AT(commands, 0) == 0) {                                                         \
                packed = (u16 *)((s32 *)(src) + 5);                                                \
                __asm__("" : "=r"(packed) : "0"(packed));                                          \
                gte_ldlv0(packed);                                                                 \
            } else {                                                                               \
                gte_ldlv0(zero0);                                                                  \
            }                                                                                      \
            gte_rt();                                                                              \
            packed = (u16 *)((s32 *)(dst) + 5);                                                    \
            gte_swc2_25_0(packed);                                                                 \
            gte_swc2_26_4(packed);                                                                 \
            gte_swc2_27_8(packed);                                                                 \
        }                                                                                          \
    }
#define Render_XformTransformVector(src, dst)                                                      \
    {                                                                                              \
        gte_ldv0((src));                                                                           \
        gte_rt();                                                                                  \
        gte_stsv((dst));                                                                           \
    }

#define Render_XformCopyMatrixFromActor(actor)                                                     \
    {                                                                                              \
        u8 *matrix;                                                             \
        u32 value;                                                              \
        matrix = (u8 *)((RenderObjectEntity *)(actor))->matrices;                                  \
        value = (u16)((RenderObjectEntity *)(actor))->model_matrix.rotation[0][0];                  \
        U16_AT(matrix, 0x20) = value;                                                              \
        matrix = (u8 *)((RenderObjectEntity *)(actor))->matrices;                                  \
        value = (u16)((RenderObjectEntity *)(actor))->model_matrix.rotation[0][1];                  \
        U16_AT(matrix, 0x22) = value;                                                              \
        matrix = (u8 *)((RenderObjectEntity *)(actor))->matrices;                                  \
        value = (u16)((RenderObjectEntity *)(actor))->model_matrix.rotation[0][2];                  \
        U16_AT(matrix, 0x24) = value;                                                              \
        matrix = (u8 *)((RenderObjectEntity *)(actor))->matrices;                                  \
        value = (u16)((RenderObjectEntity *)(actor))->model_matrix.rotation[1][0];                  \
        U16_AT(matrix, 0x26) = value;                                                              \
        matrix = (u8 *)((RenderObjectEntity *)(actor))->matrices;                                  \
        value = (u16)((RenderObjectEntity *)(actor))->model_matrix.rotation[1][1];                  \
        U16_AT(matrix, 0x28) = value;                                                              \
        matrix = (u8 *)((RenderObjectEntity *)(actor))->matrices;                                  \
        value = (u16)((RenderObjectEntity *)(actor))->model_matrix.rotation[1][2];                  \
        U16_AT(matrix, 0x2a) = value;                                                              \
        matrix = (u8 *)((RenderObjectEntity *)(actor))->matrices;                                  \
        value = (u16)((RenderObjectEntity *)(actor))->model_matrix.rotation[2][0];                  \
        U16_AT(matrix, 0x2c) = value;                                                              \
        matrix = (u8 *)((RenderObjectEntity *)(actor))->matrices;                                  \
        value = (u16)((RenderObjectEntity *)(actor))->model_matrix.rotation[2][1];                  \
        U16_AT(matrix, 0x2e) = value;                                                              \
        matrix = (u8 *)((RenderObjectEntity *)(actor))->matrices;                                  \
        value = (u16)((RenderObjectEntity *)(actor))->model_matrix.rotation[2][2];                  \
        U16_AT(matrix, 0x30) = value;                                                              \
        matrix = (u8 *)((RenderObjectEntity *)(actor))->matrices;                                  \
        value = ((RenderObjectEntity *)(actor))->model_matrix.translation[0];                      \
        U32_AT(matrix, 0x34) = value;                                                              \
        matrix = (u8 *)((RenderObjectEntity *)(actor))->matrices;                                  \
        value = ((RenderObjectEntity *)(actor))->model_matrix.translation[1];                      \
        U32_AT(matrix, 0x38) = value;                                                              \
        matrix = (u8 *)((RenderObjectEntity *)(actor))->matrices;                                  \
        value = ((RenderObjectEntity *)(actor))->model_matrix.translation[2];                      \
        U32_AT(matrix, 0x3c) = value;                                                              \
    }

#define Render_XformUpdateMode4Matrix(actor)                                                       \
    {                                                                                              \
        register u8 *parent asm("$3");                                                             \
        s32 *parent_matrix;                                                     \
        s32 *actor_matrix;                                                      \
        s32 bone_index;                                                         \
                                                                                                   \
        parent = PTR_AT((actor), 0x24);                                                            \
        bone_index = S16_AT((actor), 0x2A);                                                        \
        parent_matrix = (s32 *)(PTR_AT(parent, 0x84) + bone_index * 0x20);                         \
        actor_matrix = (s32 *)((actor) + 0x34);                                                    \
                                                                                                   \
        \
         \
        Render_XformLoadRotMatrix(parent_matrix);                                                  \
        \
        {                                                                                          \
            u16 *column = (u16 *)((actor) + 0x34);                              \
            \
            asm volatile("" : "=r"(column) : "0"(column)); \
            Render_XformLoadAxis(column);                                                          \
            Render_XformStoreAxis((s16 *)column);                                                  \
        }                                                                                          \
        {                                                                                          \
            u16 *column = (u16 *)((actor) + 0x36);                              \
            \
            asm volatile("" : "=r"(column) : "0"(column)); \
            Render_XformLoadAxis(column);                                                          \
            Render_XformStoreAxis((s16 *)column);                                                  \
        }                                                                                          \
        {                                                                                          \
            u16 *column = (u16 *)((actor) + 0x38);                              \
            \
            asm volatile("" : "=r"(column) : "0"(column)); \
            Render_XformLoadAxis(column);                                                          \
            Render_XformStoreAxis((s16 *)column);                                                  \
        }                                                                                          \
        parent = PTR_AT(actor, 0x24);                                                              \
        bone_index = S16_AT(actor, 0x2A);                                                          \
        parent_matrix = (s32 *)(PTR_AT(parent, 0x84) + bone_index * 0x20);                         \
         \
        Render_XformLoadTrans(parent_matrix);                                                      \
        {                                                                                          \
            u16 *packed = (u16 *)((actor) + 0x48);                              \
            __asm__("" : "=r"(packed) : "0"(packed));                                              \
            gte_ldlv0(packed);                                                                     \
            gte_rt();                                                                              \
            gte_stlvl(packed);                                                                     \
        }                                                                                          \
    }

void Render_TransformVertices(RenderObjectEntity *input) {
    u8 *actor = (u8 *)input;
    s32 *zero0 = (s32 *)0x1F800000;
    register s32 *zero4 asm("$18") = (s32 *)0x1F800004;
    s32 *zero8 = (s32 *)0x1F800008;
    s32 *matrix_stack = (s32 *)0x1F80000C;
    register s32 *stack_top asm("$17");
    u8 *commands;
    register u8 *out_matrix asm("$6");
    register u8 *out_vertices asm("$9");
    u8 *header;
    s32 *current_matrix;
    s32 *src_matrix;
    u32 frameReserve[2];
    int mode;
    int part_count;
    s32 i;
    s32 command;

    mode = S16_AT(actor, 0x28);
    if (mode == 1) {
        Render_CopyFrameData(input, ((RenderObjectEntity *)actor)->animation_source,
                             ((RenderObjectEntity *)actor)->animation_id);
        current_matrix = (s32 *)(actor + 0x34);
    } else if (mode == 3) {
        if ((U16_AT(actor, 0x9C) & 0x400) != 0) {
            Render_XformCopyMatrixFromActor(actor);
        } else {
            Render_CopyFrameDataDouble((RenderObjectEntity *)actor,
                                       ((RenderObjectEntity *)actor)->animation_source,
                                       ((RenderObjectEntity *)actor)->animation_id);
        }
        return;
    } else {
        if (mode == 4) {
            Render_XformUpdateMode4Matrix(actor);
        }
        current_matrix = (s32 *)(actor + 0x34);
    }

    *zero0 = 0;
    *zero4 = 0;
    stack_top = matrix_stack;
    out_matrix = (u8 *)((RenderObjectEntity *)actor)->matrices;
    commands = (u8 *)((RenderObjectEntity *)actor)->matrix_commands;
    out_vertices = PTR_AT(actor, 0x80);

    Render_XformLoadFullMatrix(current_matrix);
    header = PTR_AT(actor, 0);
    part_count = U16_AT(header, 0x18);
    i = 0;
    if (part_count > 0) {
        do {
            command = S8_AT(commands, 0);
            if (command == -1) {
                Render_XformStoreFullMatrix(stack_top);
                stack_top += 8;
                continue;
            }

            if (command == -2) {
                stack_top -= 8;
                Render_XformLoadFullMatrix(stack_top);
                continue;
            }

            {
                s32 byteOffset = command * 0x20;
                u8 *matrixBase = PTR_AT(actor, 0x58);
                src_matrix = (s32 *)(matrixBase + byteOffset);
            }
            Render_XformBuildChildMatrix(src_matrix, (s32 *)out_matrix, command == 0);

            Render_XformLoadFullMatrix((s32 *)out_matrix);
            out_matrix += 0x20;
            {
                s32 visibleCommand = S8_AT(commands, 0);
                RenderObjectPart *parts = ((RenderObjectEntity *)actor)->parts;
                s32 partOffset = visibleCommand * 12;
                RenderObjectPart *part =
                    (RenderObjectPart *)(partOffset + (u32)parts);
                if (part->visible == 1) {
                    RenderVec3s *base =
                        ((RenderObjectEntity *)actor)->bounds_vertices;
                    s32 boundsOffset = visibleCommand * 16;
                    RenderVec3s *bounds =
                        (RenderVec3s *)(boundsOffset + (u32)base);
                    if (bounds[1].pad >= 0) {
                        Render_XformTransformVector(bounds, out_vertices);
                        out_vertices += 12;
                    }
                }
            }
        } while (i++, commands++, i < U16_AT(PTR_AT(actor, 0), 0x18));
    }
}

/* Matching debt: register pins, empty compiler barriers, physical scratchpad
 * accesses and native -G8 compiler/assembler addressing. GTE operations are
 * individually wrapped in the shared header. No CPU instruction ASM. */

#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))

#define Render_SkinnedLoadRotMatrix(matrix) gte_ldrotmatrix((matrix))

#define Render_SkinnedLoadTrans(matrix) gte_ldtransmatrix((matrix))

#define Render_SkinnedLoadFullMatrix(matrix)                                                       \
    {                                                                                              \
        Render_SkinnedLoadRotMatrix(matrix);                                                       \
        Render_SkinnedLoadTrans(matrix);                                                           \
    }

#define Render_SkinnedTransformVec(src, dst)                                                       \
    {                                                                                              \
        gte_ldv0(src);                                                                             \
        gte_rt();                                                                                  \
        {                                                                                          \
            u8 *output = (u8 *)(dst);                                           \
            asm("" : "=r"(output) : "0"(output));                                                  \
            gte_stsv(output);                                                                      \
        }                                                                                          \
    }

#define Skinned_LoadAxis(src) \
    { \
        gte_ldclmv((src)); \
        gte_rtir(); \
    }

#define Skinned_StoreAxis(dst) gte_stclmv((dst))

#define Skinned_RootAxis(actor)                                                                    \
    {                                                                                              \
        register u16 *src asm("$9") = (u16 *)(u8 *)actor->matrices;                                \
        Skinned_LoadAxis(src);                                                                     \
    }
#define Skinned_SelectedAxis(actor)                                                                \
    {                                                                                              \
        int index = (s16)actor->table_index;                                    \
        u16 *src;                                                               \
        src = (u16 *)((u8 *)actor->matrices + index * 32);                                         \
        Skinned_LoadAxis(src);                                                                     \
    }
#define Render_SkinnedBuildMatrix(view_matrix, bone_expr, out_matrix, first_axis)                  \
    {                                                                                              \
        Render_SkinnedLoadRotMatrix(view_matrix);                                                  \
        first_axis;                                                                                \
        Skinned_StoreAxis((s16 *)(out_matrix));                                                    \
        {                                                                                          \
            u16 *src = (u16 *)(bone_expr) + 1;                                  \
            asm volatile("" : "=r"(src) : "0"(src)); \
            Skinned_LoadAxis(src);                                                                 \
            {                                                                                      \
                register s16 *dst asm("$9") = (s16 *)0x1F800002;                                   \
                asm volatile("" : "=r"(dst) : "0"(dst)); \
                Skinned_StoreAxis(dst);                                                            \
            }                                                                                      \
        }                                                                                          \
        {                                                                                          \
            u16 *src = (u16 *)(bone_expr) + 2;                                  \
            asm volatile("" : "=r"(src) : "0"(src)); \
            Skinned_LoadAxis(src);                                                                 \
            {                                                                                      \
                register s16 *dst asm("$9") = (s16 *)0x1F800004;                                   \
                asm volatile("" : "=r"(dst) : "0"(dst)); \
                Skinned_StoreAxis(dst);                                                            \
            }                                                                                      \
        }                                                                                          \
        Render_SkinnedLoadTrans(view_matrix);                                                      \
        {                                                                                          \
            u8 *src = (u8 *)(bone_expr) + 20;                                   \
            asm("" : "=r"(src) : "0"(src));                                                        \
            gte_ldlv0(src);                                                                        \
            gte_rt();                                                                              \
            {                                                                                      \
                register s32 *dst asm("$9") = (s32 *)0x1F800014;                                   \
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
            gte_stsxy2(output);                                                                    \
        }                                                                                          \
    }

void Render_TransformSkinnedVertices(RenderObjectEntity *input, u32 *view_input) {
    RenderObjectEntity *actor = input;
    s32 *view_matrix = (s32 *)view_input;
    u16 *scratch_vec = (u16 *)0x1F800020;
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
        Render_SkinnedTransformVec(vector, &actor->hit_cylinder.value0);
    }
    {
        register s32 *matrix asm("$9") = (s32 *)(u8 *)actor->matrices;
        Render_SkinnedLoadFullMatrix(matrix);
    }
    scratch_vec[0] = U16_AT(actor->model_section14, 0) + (u16)actor->hit_body.value0;
    *(u16 *)0x1F800022 = U16_AT(actor->model_section14, 2) + (u16)actor->hit_body.value1;
    asm volatile("" : : : "memory");
    *(u16 *)0x1F800024 = U16_AT(actor->model_section14, 4) + (u16)actor->hit_body.value2;
    Render_SkinnedTransformVec(scratch_vec, &actor->hit_body.value0);
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

/* Matching debt: register pins, empty compiler barriers, raw pointer casts
 * and an artificial two-word frame reserve. GTE instructions and their
 * authorized hazard nops are individually wrapped in the shared header. */
#define MorphLoadRotMatrix(matrix)                                                                 \
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

#define MorphLoadTrans(matrix) \
    { \
        register u32 x asm("$12"), y asm("$13"), z asm("$14"); \
        x = (matrix)[5]; \
        y = (matrix)[6]; \
        gte_ctc2_5(x); \
        z = (matrix)[7]; \
        gte_ctc2_6(y); \
        gte_ctc2_7(z); \
    }

#define MorphLoadFullMatrix(matrix)                                                                \
    {                                                                                              \
        MorphLoadRotMatrix(matrix);                                                                \
        MorphLoadTrans(matrix);                                                                    \
    }

#define Morph_LoadAxis(src)                                                                        \
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
#define Morph_StoreAxis(dst)                                                                       \
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

#define Morph_Compose(view, matrix, scratch)                                                       \
    {                                                                                              \
        MorphLoadRotMatrix(view);                                                                  \
        Morph_LoadAxis((u16 *)(matrix));                                                           \
        Morph_StoreAxis((s16 *)(scratch));                                                         \
        asm volatile("" : : : "memory");                                                           \
        {                                                                                          \
            register u16 *src asm("$2") = (u16 *)(matrix) + 1;                                     \
            asm("" : "=r"(src) : "0"(src));                                                        \
            Morph_LoadAxis(src);                                                                   \
            src = (u16 *)(scratch) + 1;                                                            \
            asm("" : "=r"(src) : "0"(src));                                                        \
            Morph_StoreAxis((s16 *)src);                                                           \
        }                                                                                          \
        {                                                                                          \
            register u16 *src asm("$2") = (u16 *)(matrix) + 2;                                     \
            asm("" : "=r"(src) : "0"(src));                                                        \
            Morph_LoadAxis(src);                                                                   \
            src = (u16 *)(scratch) + 2;                                                            \
            asm("" : "=r"(src) : "0"(src));                                                        \
            Morph_StoreAxis((s16 *)src);                                                           \
        }                                                                                          \
        MorphLoadTrans(view);                                                                      \
        {                                                                                          \
            u8 *src = (u8 *)(matrix) + 20;                                                         \
            register u32 xy asm("$12");                                                            \
            register u32 y asm("$13");                                                             \
            asm("" : "=r"(src) : "0"(src));                                                        \
            y = *(u16 *)(src + 4);                                                                 \
            xy = *(u16 *)src;                                                                      \
            y <<= 16;                                                                              \
            xy |= y;                                                                               \
            gte_mtc2_0(xy);                                                                        \
            gte_lwc2_1_8(src);                                                                     \
            gte_cop2_hazard_slot();                                                                \
            gte_cop2_hazard_slot();                                                                \
            gte_mvmva_rotation_v0_translation_sf12();                                              \
            src = (u8 *)(scratch) + 20;                                                            \
                                                                                                   \
            gte_swc2_25_0(src);                                                                    \
            gte_swc2_26_4(src);                                                                    \
            gte_swc2_27_8(src);                                                                    \
        }                                                                                          \
    }
void Render_TransformMorphVertices(RenderObjectEntity *input, u32 *view_input) {
    u32 frame[2];
    RenderObjectEntity *entity = input;
    s32 *view = (s32 *)view_input;
    int i;
    register s32 *matrix asm("$10");
    register s32 *scratch asm("$24");
    int offset;
    asm("" : "=r"(entity), "=r"(view) : "0"(entity), "1"(view), "m"(frame[0]));
    i = 0;
    asm("" : : "r"(i));
    {
        int count = entity->header->part_count;
        matrix = (s32 *)entity->matrices;
        scratch = (s32 *)0x1F800000;
        if (0 < count) {
            offset = 0;
            asm("" : "=r"(scratch) : "0"(scratch));
            do {
                RenderObjectPart *base = entity->parts;
                RenderObjectPart *part = (RenderObjectPart *)((u8 *)base + offset);
                asm volatile("" : "=r"(part) : "0"(part) : "memory");
                Morph_Compose(view, matrix, scratch);
                MorphLoadFullMatrix(scratch);
                matrix += 8;
                if (part->visible == 1) {
                    int j = 0;
                    int first;
                    RenderVec3s *base_vertices;
                    register int vertex_offset asm("$2");
                    RenderVec3s *vertices;
                    register u32 *screen asm("$6");
                    u32 *depth;
                    int count;
                    first = part->vertex_start;
                    base_vertices = entity->vertices;
                    vertex_offset = first * 8;
                    vertices = (RenderVec3s *)((u8 *)base_vertices + vertex_offset);
                    first <<= 2;
                    {
                        register u32 *base_screen asm("$2") = D_800B1638;

                        screen = (u32 *)(first + (u32)base_screen);
                    }
                    {
                        u32 *base_depth = D_800A6360;

                        count = part->vertex_count;
                        depth = (u32 *)(first + (u32)base_depth);
                    }
                    if (j < count) {
                        do {
                            gte_lwc2_0_0(vertices);
                            gte_lwc2_1_4(vertices);
                            gte_lwc2_2_8(vertices);
                            gte_lwc2_3_12(vertices);
                            gte_lwc2_4_16(vertices);
                            gte_lwc2_5_20(vertices);
                            gte_cop2_hazard_slot();
                            gte_cop2_hazard_slot();
                            gte_rtpt_command();
                            vertices += 3;
                            j += 3;
                            screen += 3;
                            depth += 3;
                            gte_swc2_12_0(screen);
                            gte_swc2_13_4(screen);
                            gte_swc2_14_8(screen);
                            gte_swc2_17_0(depth);
                            gte_swc2_18_4(depth);
                            gte_swc2_19_8(depth);
                        } while (j < part->vertex_count);
                    }
                }
                offset += sizeof(RenderObjectPart);
                i++;
            } while (i < entity->header->part_count);
        }
    }
}
