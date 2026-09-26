/* Matching debt: register pins, empty compiler barriers, raw pointer casts
 * and an artificial two-word frame reserve. GTE instructions and their
 * authorized hazard nops are individually wrapped in the shared header. */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_object.h"
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

#define MorphLoadTrans(matrix)                                                                     \
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
                register RenderObjectPart *base asm("$2") = entity->parts;
                RenderObjectPart *part = (RenderObjectPart *)((u8 *)base + offset);
                asm volatile("" : "=r"(part) : "0"(part) : "memory");
                Morph_Compose(view, matrix, scratch);
                MorphLoadFullMatrix(scratch);
                matrix += 8;
                if (part->visible == 1) {
                    int j = 0;
                    int first;
                    register RenderVec3s *base_vertices asm("$4");
                    register int vertex_offset asm("$2");
                    RenderVec3s *vertices;
                    register u32 *screen asm("$6");
                    u32 *depth;
                    int count;
                    first = part->vertex_start;
                    base_vertices = entity->vertices;
                    vertex_offset = first * 8;
                    asm(""
                        : "=r"(first), "=r"(base_vertices), "=r"(vertex_offset)
                        : "0"(first), "1"(base_vertices), "2"(vertex_offset));
                    vertices = (RenderVec3s *)((u8 *)base_vertices + vertex_offset);
                    first <<= 2;
                    {
                        register u32 *base_screen asm("$2") = D_800B1638;

                        screen = (u32 *)((u32)first + (u32)base_screen);
                    }
                    {
                        register u32 *base_depth asm("$2") = D_800A6360;

                        count = part->vertex_count;
                        depth = (u32 *)((u32)first + (u32)base_depth);
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
