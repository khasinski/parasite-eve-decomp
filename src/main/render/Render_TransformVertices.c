/* Matching debt: pins, empty barriers, volatile accesses, raw-offset
 * accessors and an artificial two-word local frame reservation. */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_object.h"

#define S8_AT(ptr, off) (*(s8 *)((u8 *)(ptr) + (off)))
#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

#define Render_XformLoadRotMatrix(matrix)                                                          \
    {                                                                                              \
        gte_ldrotmatrix(matrix); \
    }

#define Render_XformLoadTrans(matrix)                                                              \
    {                                                                                              \
        gte_ldtransmatrix(matrix); \
    }

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
        gte_ldclmv(src); gte_rtir(); \
        *zero8 =                                                                                   \
            ((RenderObjectPart *)((RenderObjectEntity *)actor)->parts)[command].translation_z; \
            gte_stclmv(dst); \
    }

#define Render_XformLoadAxis(src)                                                                  \
    { gte_ldclmv(src); gte_rtir(); }
#define Render_XformStoreAxis(dst)                                                                 \
    {                                                                                              \
        gte_stclmv(dst); \
    }



#define Render_XformBuildChildMatrix(src, dst, include_translation)                                \
    {                                                                                              \
        Render_XformTransformAxisZ((u16 *)(src), (s16 *)(dst));                                    \
        \
        {                                                                                          \
            u16 *column = (u16 *)(src) + 1;                                     \
            \
            Render_XformLoadAxis(column);                                                          \
            column = (u16 *)(dst) + 1;                                                             \
            \
            Render_XformStoreAxis((s16 *)column);                                                  \
        }                                                                                          \
        {                                                                                          \
            register u16 *column asm("$2") = (u16 *)(src) + 2;                                     \
            \
            Render_XformLoadAxis(column);                                                          \
            column = (u16 *)(dst) + 2;                                                             \
            \
            Render_XformStoreAxis((s16 *)column);                                                  \
        }                                                                                          \
        {                                                                                          \
            register u32 x asm("$12"), y asm("$13");                                               \
            u16 *packed;                                                        \
            if (S8_AT(commands, 0) == 0) {                                                         \
                packed = (u16 *)((s32 *)(src) + 5);                                                \
                __asm__("" : "=r"(packed) : "0"(packed));                                          \
                y = packed[2];                                                                     \
                x = packed[0];                                                                     \
                y <<= 16;                                                                          \
                x |= y;                                                                            \
                gte_mtc2_0(x);                                                                     \
                gte_lwc2_1_8(packed);                                                              \
            } else {                                                                               \
                y = ((volatile u16 *)zero0)[2];                                                    \
                x = ((volatile u16 *)zero0)[0];                                                    \
                y <<= 16;                                                                          \
                x |= y;                                                                            \
                gte_mtc2_0(x);                                                                     \
                gte_lwc2_1_8(zero0);                                                               \
            }                                                                                      \
            gte_cop2_hazard_slot();                                                                \
            gte_cop2_hazard_slot();                                                                \
            gte_mvmva_rotation_v0_translation_sf12();                                              \
            packed = (u16 *)((s32 *)(dst) + 5);                                                    \
            gte_swc2_25_0(packed);                                                                 \
            gte_swc2_26_4(packed);                                                                 \
            gte_swc2_27_8(packed);                                                                 \
        }                                                                                          \
    }
#define Render_XformTransformVector(src, dst)                                                      \
    {                                                                                              \
        register s32 x asm("$12");                                                                 \
        register s32 y asm("$13");                                                                 \
        register s32 z asm("$14");                                                                 \
                                                                                                   \
        gte_lwc2_0_0((src));                                                                       \
        gte_lwc2_1_4((src));                                                                       \
        gte_cop2_hazard_slot();                                                                    \
        gte_cop2_hazard_slot();                                                                    \
        gte_mvmva_rotation_v0_translation_sf12();                                                  \
        gte_mfc2_9(x);                                                                             \
        gte_mfc2_10(y);                                                                            \
        gte_mfc2_11(z);                                                                            \
                                                                                                   \
        S16_AT((dst), 0) = x;                                                                      \
        S16_AT((dst), 2) = y;                                                                      \
        S16_AT((dst), 4) = z;                                                                      \
    }

#define Render_XformCopyMatrixFromActor(actor)                                                     \
    {                                                                                              \
        u8 *matrix;                                                             \
        u32 value;                                                              \
        matrix = *(u8 *volatile *)((u8 *)(actor) + 0x84);                                          \
        value = U16_AT(actor, 0x34);                                                               \
        U16_AT(matrix, 0x20) = value;                                                              \
        matrix = *(u8 *volatile *)((u8 *)(actor) + 0x84);                                          \
        value = U16_AT(actor, 0x36);                                                               \
        U16_AT(matrix, 0x22) = value;                                                              \
        matrix = *(u8 *volatile *)((u8 *)(actor) + 0x84);                                          \
        value = U16_AT(actor, 0x38);                                                               \
        U16_AT(matrix, 0x24) = value;                                                              \
        matrix = *(u8 *volatile *)((u8 *)(actor) + 0x84);                                          \
        value = U16_AT(actor, 0x3a);                                                               \
        U16_AT(matrix, 0x26) = value;                                                              \
        matrix = *(u8 *volatile *)((u8 *)(actor) + 0x84);                                          \
        value = U16_AT(actor, 0x3c);                                                               \
        U16_AT(matrix, 0x28) = value;                                                              \
        matrix = *(u8 *volatile *)((u8 *)(actor) + 0x84);                                          \
        value = U16_AT(actor, 0x3e);                                                               \
        U16_AT(matrix, 0x2a) = value;                                                              \
        matrix = *(u8 *volatile *)((u8 *)(actor) + 0x84);                                          \
        value = U16_AT(actor, 0x40);                                                               \
        U16_AT(matrix, 0x2c) = value;                                                              \
        matrix = *(u8 *volatile *)((u8 *)(actor) + 0x84);                                          \
        value = U16_AT(actor, 0x42);                                                               \
        U16_AT(matrix, 0x2e) = value;                                                              \
        matrix = *(u8 *volatile *)((u8 *)(actor) + 0x84);                                          \
        value = U16_AT(actor, 0x44);                                                               \
        U16_AT(matrix, 0x30) = value;                                                              \
        matrix = *(u8 *volatile *)((u8 *)(actor) + 0x84);                                          \
        value = U32_AT(actor, 0x48);                                                               \
        U32_AT(matrix, 0x34) = value;                                                              \
        matrix = *(u8 *volatile *)((u8 *)(actor) + 0x84);                                          \
        value = U32_AT(actor, 0x4c);                                                               \
        U32_AT(matrix, 0x38) = value;                                                              \
        matrix = *(u8 *volatile *)((u8 *)(actor) + 0x84);                                          \
        value = U32_AT(actor, 0x50);                                                               \
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
        Render_XformLoadRotMatrix(parent_matrix);                                                  \
        \
        {                                                                                          \
            u16 *column = (u16 *)((actor) + 0x34);                              \
            \
            Render_XformLoadAxis(column);                                                          \
            Render_XformStoreAxis((s16 *)column);                                                  \
        }                                                                                          \
        {                                                                                          \
            u16 *column = (u16 *)((actor) + 0x36);                              \
            \
            Render_XformLoadAxis(column);                                                          \
            Render_XformStoreAxis((s16 *)column);                                                  \
        }                                                                                          \
        {                                                                                          \
            u16 *column = (u16 *)((actor) + 0x38);                              \
            \
            Render_XformLoadAxis(column);                                                          \
            Render_XformStoreAxis((s16 *)column);                                                  \
        }                                                                                          \
        parent = PTR_AT(actor, 0x24);                                                              \
        bone_index = S16_AT(actor, 0x2A);                                                          \
        parent_matrix = (s32 *)(PTR_AT(parent, 0x84) + bone_index * 0x20);                         \
        Render_XformLoadTrans(parent_matrix);                                                      \
        {                                                                                          \
            u16 *packed = (u16 *)((actor) + 0x48);                              \
            register u32 xy asm("$12"), high asm("$13");                                           \
            __asm__("" : "=r"(packed) : "0"(packed));                                              \
            high = packed[2];                                                                      \
            xy = packed[0];                                                                        \
            high <<= 16;                                                                           \
            xy |= high;                                                                            \
            gte_mtc2_0(xy);                                                                        \
            gte_lwc2_1_8(packed);                                                                  \
            gte_cop2_hazard_slot();                                                                \
            gte_cop2_hazard_slot();                                                                \
            gte_mvmva_rotation_v0_translation_sf12();                                              \
            gte_swc2_9_0(packed);                                                                  \
            gte_swc2_10_4(packed);                                                                 \
            gte_swc2_11_8(packed);                                                                 \
        }                                                                                          \
    }

void Render_TransformVertices(RenderObjectEntity *input) {
    u8 *actor = (u8 *)input;
    volatile s32 *zero0 = (volatile s32 *)0x1F800000;
    register volatile s32 *zero4 asm("$18") = (volatile s32 *)0x1F800004;
    volatile s32 *zero8 = (volatile s32 *)0x1F800008;
    s32 *matrix_stack = (s32 *)0x1F80000C;
    register s32 *stack_top asm("$17");
    u8 *commands;
    u8 *out_matrix;
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
    out_matrix = PTR_AT(actor, 0x84);
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
                    (RenderObjectPart *)((u32)partOffset + (u32)parts);
                if (part->visible == 1) {
                    RenderVec3s *base =
                        ((RenderObjectEntity *)actor)->bounds_vertices;
                    s32 boundsOffset = visibleCommand * 16;
                    RenderVec3s *bounds =
                        (RenderVec3s *)((u32)boundsOffset + (u32)base);
                    if (bounds[1].pad >= 0) {
                        Render_XformTransformVector(bounds, out_vertices);
                        out_vertices += 12;
                    }
                }
            }
        } while (i++, commands++, i < U16_AT(PTR_AT(actor, 0), 0x18));
    }
}
