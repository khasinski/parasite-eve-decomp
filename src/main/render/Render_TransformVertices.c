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
        register s32 x asm("$12"), y asm("$13"), z asm("$14");                                     \
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

#define Render_XformLoadTrans(matrix)                                                              \
    {                                                                                              \
        register s32 x asm("$12"), y asm("$13"), z asm("$14");                                     \
        x = (matrix)[5];                                                                           \
        y = (matrix)[6];                                                                           \
        gte_ctc2_5(x);                                                                             \
        z = (matrix)[7];                                                                           \
        gte_ctc2_6(y);                                                                             \
        gte_ctc2_7(z);                                                                             \
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
        register s32 x asm("$12");                                                                 \
        register s32 y asm("$13");                                                                 \
        register s32 z asm("$14");                                                                 \
                                                                                                   \
        x = (src)[0];                                                                              \
        y = (src)[3];                                                                              \
        z = (src)[6];                                                                              \
        gte_mtc2_9(x);                                                                             \
        gte_mtc2_10(y);                                                                            \
        gte_mtc2_11(z);                                                                            \
        gte_cop2_hazard_slot();                                                                    \
        gte_cop2_hazard_slot();                                                                    \
        gte_mvmva_rotation_ir_sf12();                                                              \
        *zero8 =                                                                                   \
            ((RenderObjectPart *)((RenderObjectEntity *)actor)->parts)[command].translation_z;     \
        gte_mfc2_9(x);                                                                             \
        gte_mfc2_10(y);                                                                            \
        gte_mfc2_11(z);                                                                            \
                                                                                                   \
        (dst)[0] = x;                                                                              \
        (dst)[3] = y;                                                                              \
        (dst)[6] = z;                                                                              \
    }

#define Render_XformLoadAxis(src)                                                                  \
    {                                                                                              \
        register s32 x asm("$12");                                                                 \
        register s32 y asm("$13");                                                                 \
        register s32 z asm("$14");                                                                 \
                                                                                                   \
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
#define Render_XformStoreAxis(dst)                                                                 \
    {                                                                                              \
        register s32 x asm("$12"), y asm("$13"), z asm("$14");                                     \
        gte_mfc2_9(x);                                                                             \
        gte_mfc2_10(y);                                                                            \
        gte_mfc2_11(z);                                                                            \
                                                                                                   \
        (dst)[0] = x;                                                                              \
        (dst)[3] = y;                                                                              \
        (dst)[6] = z;                                                                              \
    }



#define Render_XformBuildChildMatrix(src, dst, include_translation)                                \
    {                                                                                              \
        Render_XformTransformAxisZ((u16 *)(src), (s16 *)(dst));                                    \
        __asm__ volatile("" : "=r"(src) : "0"(src), "m"(((s16 *)(dst))[6]));                       \
        {                                                                                          \
            register u16 *column asm("$2") = (u16 *)(src) + 1;                                     \
            __asm__("" : "=r"(column) : "0"(column));                                              \
            Render_XformLoadAxis(column);                                                          \
            column = (u16 *)(dst) + 1;                                                             \
            __asm__("" : "=r"(column) : "0"(column));                                              \
            Render_XformStoreAxis((s16 *)column);                                                  \
        }                                                                                          \
        {                                                                                          \
            register u16 *column asm("$2") = (u16 *)(src) + 2;                                     \
            __asm__("" : "=r"(column) : "0"(column));                                              \
            Render_XformLoadAxis(column);                                                          \
            column = (u16 *)(dst) + 2;                                                             \
            __asm__("" : "=r"(column) : "0"(column));                                              \
            Render_XformStoreAxis((s16 *)column);                                                  \
        }                                                                                          \
        {                                                                                          \
            register u32 x asm("$12"), y asm("$13");                                               \
            register u16 *packed asm("$2");                                                        \
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
        register u8 *matrix asm("$3");                                                             \
        register u32 value asm("$2");                                                              \
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
        register s32 *parent_matrix asm("$3");                                                     \
        register s32 *actor_matrix asm("$2");                                                      \
        register s32 bone_index asm("$2");                                                         \
                                                                                                   \
        parent = PTR_AT((actor), 0x24);                                                            \
        bone_index = S16_AT((actor), 0x2A);                                                        \
        parent_matrix = (s32 *)(PTR_AT(parent, 0x84) + bone_index * 0x20);                         \
        actor_matrix = (s32 *)((actor) + 0x34);                                                    \
                                                                                                   \
        __asm__("" : "=r"(actor_matrix) : "0"(actor_matrix));                                      \
        Render_XformLoadRotMatrix(parent_matrix);                                                  \
        __asm__ volatile("" : "=r"(actor) : "0"(actor), "r"(parent_matrix));                       \
        {                                                                                          \
            register u16 *column asm("$2") = (u16 *)((actor) + 0x34);                              \
            __asm__("" : "=r"(column) : "0"(column));                                              \
            Render_XformLoadAxis(column);                                                          \
            Render_XformStoreAxis((s16 *)column);                                                  \
        }                                                                                          \
        {                                                                                          \
            register u16 *column asm("$2") = (u16 *)((actor) + 0x36);                              \
            __asm__("" : "=r"(column) : "0"(column));                                              \
            Render_XformLoadAxis(column);                                                          \
            Render_XformStoreAxis((s16 *)column);                                                  \
        }                                                                                          \
        {                                                                                          \
            register u16 *column asm("$2") = (u16 *)((actor) + 0x38);                              \
            __asm__("" : "=r"(column) : "0"(column));                                              \
            Render_XformLoadAxis(column);                                                          \
            Render_XformStoreAxis((s16 *)column);                                                  \
        }                                                                                          \
        parent = PTR_AT(actor, 0x24);                                                              \
        bone_index = S16_AT(actor, 0x2A);                                                          \
        parent_matrix = (s32 *)(PTR_AT(parent, 0x84) + bone_index * 0x20);                         \
        Render_XformLoadTrans(parent_matrix);                                                      \
        {                                                                                          \
            register u16 *packed asm("$2") = (u16 *)((actor) + 0x48);                              \
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
    register u8 *actor asm("$16") = (u8 *)input;
    register volatile s32 *zero0 asm("$19") = (volatile s32 *)0x1F800000;
    register volatile s32 *zero4 asm("$18") = (volatile s32 *)0x1F800004;
    register volatile s32 *zero8 asm("$20") = (volatile s32 *)0x1F800008;
    s32 *matrix_stack = (s32 *)0x1F80000C;
    register s32 *stack_top asm("$17");
    register u8 *commands asm("$7");
    register u8 *out_matrix asm("$6");
    register u8 *out_vertices asm("$9");
    u8 *header;
    register s32 *current_matrix asm("$2");
    register s32 *src_matrix asm("$5");
    u32 frameReserve[2];
    int mode;
    int part_count;
    register s32 i asm("$8");
    register s32 command asm("$4");

    __asm__("" : "=r"(actor) : "0"(actor));
    mode = S16_AT(actor, 0x28);
    if (mode == 1) {
        Render_CopyFrameData(input, ((RenderObjectEntity *)actor)->animation_source,
                             ((RenderObjectEntity *)actor)->animation_id);
        current_matrix = (s32 *)(actor + 0x34);
    } else if (mode == 3) {
        __asm__("" : : : "$4");
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
            __asm__("" : "=r"(command) : "0"(command));
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
                register s32 byteOffset asm("$2") = command * 0x20;
                register u8 *matrixBase asm("$3") = PTR_AT(actor, 0x58);
                src_matrix = (s32 *)(matrixBase + byteOffset);
            }
            Render_XformBuildChildMatrix(src_matrix, (s32 *)out_matrix, command == 0);
            Render_XformLoadFullMatrix((s32 *)out_matrix);
            out_matrix += 0x20;
            {
                register s32 visibleCommand asm("$4") = S8_AT(commands, 0);
                register RenderObjectPart *parts asm("$3") = ((RenderObjectEntity *)actor)->parts;
                register s32 partOffset asm("$2") = visibleCommand * 12;
                register RenderObjectPart *part asm("$2") =
                    (RenderObjectPart *)((u32)partOffset + (u32)parts);
                if (part->visible == 1) {
                    register RenderVec3s *base asm("$2") =
                        ((RenderObjectEntity *)actor)->bounds_vertices;
                    register s32 boundsOffset asm("$3") = visibleCommand * 16;
                    register RenderVec3s *bounds asm("$3") =
                        (RenderVec3s *)((u32)boundsOffset + (u32)base);
                    if (bounds[1].pad >= 0) {
                        Render_XformTransformVector(bounds, out_vertices);
                        out_vertices += 12;
                    }
                }
            }
        } while (i++, commands++, i < U16_AT(PTR_AT(actor, 0), 0x18));
    }
    __asm__("" : "=m"(frameReserve));
}
