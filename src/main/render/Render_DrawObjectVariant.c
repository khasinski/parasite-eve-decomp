#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_object.h"
extern u32 D_800B1640[];
#define Render_LoadObjectMatrix(matrix)                                                            \
    {                                                                                              \
        register s32 w0 asm("$12");                                                                \
        register s32 w1 asm("$13");                                                                \
        register s32 w2 asm("$14");                                                                \
        w0 = matrix[0];                                                                            \
        w1 = matrix[1];                                                                            \
        gte_ctc2_0(w0);                                                                            \
        gte_ctc2_1(w1);                                                                            \
        w0 = matrix[2];                                                                            \
        w1 = matrix[3];                                                                            \
        w2 = matrix[4];                                                                            \
        gte_ctc2_2(w0);                                                                            \
        gte_ctc2_3(w1);                                                                            \
        gte_ctc2_4(w2);                                                                            \
        w0 = matrix[5];                                                                            \
        w1 = matrix[6];                                                                            \
        gte_ctc2_5(w0);                                                                            \
        w2 = matrix[7];                                                                            \
        gte_ctc2_6(w1);                                                                            \
        gte_ctc2_7(w2);                                                                            \
    }

#define Render_TransformVertex(src, dst)                                                           \
    {                                                                                              \
        register s32 x asm("$12");                                                                 \
        register s32 y asm("$13");                                                                 \
        register s32 z asm("$14");                                                                 \
                                                                                                   \
        gte_lwc2_0_0(src);                                                                         \
        gte_lwc2_1_4(src);                                                                         \
        gte_cop2_hazard_slot();                                                                    \
        gte_cop2_hazard_slot();                                                                    \
        gte_mvmva_rotation_v0_translation_sf12();                                                  \
        gte_mfc2_9(x);                                                                             \
        gte_mfc2_10(y);                                                                            \
        gte_mfc2_11(z);                                                                            \
                                                                                                   \
        dst->x = x;                                                                                \
        dst->y = y;                                                                                \
        dst->z = z;                                                                                \
    }

void Render_DrawObjectVariant(RenderObjectEntity *input, s16 limit, s32 *projectionMatrix) {
    register RenderObjectEntity *entity asm("$7") = input;
    register s32 *projectMatrix asm("$16") = projectionMatrix;
    register s16 savedLimit asm("$17") = limit;
    register s32 changed asm("$15");
    register volatile RenderVec3s *scratch asm("$24") = (volatile RenderVec3s *)0x1F800000;
    register volatile u32 *projected asm("$6");
    register s32 *matrix asm("$8");
    register s32 i asm("$11");
    register s32 offset asm("$25");
    register s32 threshold asm("$10");
    register RenderObjectPart *part asm("$9");
    register RenderObjectPart *probe asm("$4");
    register s32 boundsOffset asm("$4");
    register RenderVec3s *bounds asm("$2");
    register s32 y asm("$3");
    register s32 radius asm("$4");
    register s32 sum asm("$2");
    register RenderVec3s *vertices asm("$4");
    register u32 *clut asm("$3");
    register s32 vertexIndex asm("$5");
    register s32 limitShift asm("$2");
    __asm__("" : "=r"(entity) : "0"(entity));
    __asm__("" : "=r"(projectMatrix) : "0"(projectMatrix));
    changed = 0;
    __asm__("" : : "r"(changed) : "$6");
    projected = (volatile u32 *)0x1F800008;
    if (!entity->header || !entity->draw_count)
        return;
    matrix = (s32 *)entity->matrices;
    i = 0;
    if (changed < entity->header->part_count) {
        limitShift = (u32)limit << 16;
        __asm__("" : "=r"(limitShift) : "0"(limitShift));
        threshold = limitShift >> 16;
        offset = 0;
        do {
            {
                register RenderObjectPart *base asm("$2") = entity->parts;
                probe = (RenderObjectPart *)((u32)offset + (u32)base);
            }
            if (probe->visible == 1) {
                part = probe;
                Render_LoadObjectMatrix(matrix);
                boundsOffset = i * 16;
                bounds = (RenderVec3s *)((u8 *)entity->bounds_vertices + boundsOffset);
                Render_TransformVertex(bounds, scratch);
                {
                    register u16 yy asm("$2") = (u16)scratch->y;
                    y = (s16)yy;
                }
                if (threshold < y ||
                    (bounds = (RenderVec3s *)((u32)boundsOffset + (u32)entity->bounds_vertices),
                     radius = bounds->pad,
                     threshold < y - radius || threshold < (sum = y + radius))) {
                    scratch->y = savedLimit;
                    Render_LoadObjectMatrix(projectMatrix);
                    gte_lwc2_0_0(scratch);
                    gte_lwc2_1_4(scratch);
                    gte_cop2_hazard_slot();
                    gte_cop2_hazard_slot();
                    gte_rtps_command();
                    gte_stsxy2(projected);
                    vertexIndex = 0;
                    {
                        register s32 first asm("$3") = part->vertex_start;
                        register RenderVec3s *base asm("$4") = entity->vertices;
                        register s32 vertexOffset asm("$2") = first * 8;
                        vertices = (RenderVec3s *)((u8 *)base + vertexOffset);
                        first <<= 2;
                        clut = (u32 *)((u32)D_800B1640 + first);
                    }
                    __asm__("" : "=r"(matrix) : "0"(matrix), "r"(clut), "r"(vertices));
                    Render_LoadObjectMatrix(matrix);
                    for (; vertexIndex < part->vertex_count; vertices++) {
                        register s32 x asm("$12");
                        register s32 yy asm("$13");
                        register s32 z asm("$14");
                        gte_lwc2_0_0(vertices);
                        gte_lwc2_1_4(vertices);
                        gte_cop2_hazard_slot();
                        gte_cop2_hazard_slot();
                        gte_mvmva_rotation_v0_translation_sf12();
                        vertexIndex++;
                        clut++;
                        gte_mfc2_9(x);
                        gte_mfc2_10(yy);
                        gte_mfc2_11(z);
                        scratch->x = x;
                        scratch->y = yy;
                        scratch->z = z;
                        if (threshold < scratch->y) {
                            *clut = *projected;
                            changed++;
                        }
                    }
                }
            }
            offset += sizeof(RenderObjectPart);
            i++;
            matrix += 8;
        } while (i < entity->header->part_count);
    }
    if (changed == entity->header->visible_part_count || (entity->flags_9C & 0x200))
        entity->variant_visible = 0;
    else
        entity->variant_visible = 1;
}
