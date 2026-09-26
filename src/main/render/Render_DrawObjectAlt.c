/* CC1_FLAGS: -fno-schedule-insns */
/* Matching debt: register pins, empty barriers, and a 16-byte local frame
 * reservation reproduce the retail allocation. The reservation is artificial.
 * GTE operations and their explicit nop hazard slots use individual macros. */

#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_object.h"

extern u8 D_800B1638[];
extern s16 D_8009CDDC;

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

void Render_DrawObjectAlt(RenderObjectEntity *input, s16 limit, u8 red, u8 green, u8 blue) {
    register RenderObjectEntity *entity asm("$9") = input;
    register u8 r asm("$17") = red;
    register u8 g asm("$16") = green;
    register u8 b asm("$7") = blue;
    register s32 threshold asm("$11");
    register volatile RenderVec3s *scratch asm("$6") = (volatile RenderVec3s *)0x1F800000;
    register RenderObjectPart *part asm("$10");
    register s32 i asm("$15");
    register s32 partOffset asm("$24");
    register int *matrix asm("$25");
    register s32 limitShift asm("$2");
    register s32 count asm("$2");
    u32 frameReserve[4];
    register s32 boundsOffset asm("$4");
    register s32 clipSum asm("$2");
    RenderVec3s *bounds;
    register s32 y asm("$3");
    register s32 radius asm("$4");
    register RenderVec3s *radiusBounds asm("$2");
    register RenderVec3s *vertices asm("$8");
    register u8 *clut asm("$3");
    register u8 *clutBlue asm("$4");
    register s32 vertexIndex asm("$5");
    if (!entity->header || !entity->draw_count)
        return;
    matrix = (int *)entity->matrices;
    i = 0;
    count = entity->header->part_count;
    if (count > 0) {
        limitShift = (u32)limit << 16;
        __asm__("" : "=r"(limitShift) : "0"(limitShift));
        threshold = limitShift >> 16;
        partOffset = 0;
        do {
            {
                register RenderObjectPart *probe asm("$2") =
                    (RenderObjectPart *)((u32)partOffset + (u32)entity->parts);
                if (probe->visible == 1) {
                    Render_LoadObjectMatrix(matrix);
                    boundsOffset = i * 16;
                    bounds = (RenderVec3s *)((u8 *)entity->bounds_vertices + boundsOffset);
                    Render_TransformVertex(bounds, scratch);
                    {
                        register RenderObjectPart *parts asm("$3") = entity->parts;
                        register u16 yy asm("$2") = (u16)scratch->y;
                        part = (RenderObjectPart *)((u8 *)parts + partOffset);
                        y = (s16)yy;
                    }
                    if (threshold < y ||
                        (radiusBounds =
                             (RenderVec3s *)((u32)boundsOffset + (u32)entity->bounds_vertices),
                         radius = radiusBounds->pad,
                         threshold < y - radius || threshold < (clipSum = y + radius))) {
                        {
                            register s32 first asm("$3") = part->vertex_start;
                            register RenderVec3s *vertexBase asm("$4") = entity->vertices;
                            register s32 vertexOffset asm("$2") = first * 8;
                            vertices = (RenderVec3s *)((u8 *)vertexBase + vertexOffset);
                            first <<= 2;
                            {
                                register u8 *clutBase asm("$2") = D_800B1638;
                                register s32 vertexCount asm("$4") = part->vertex_count;
                                clut = clutBase + first;
                                vertexIndex = 0;
                                if (vertexCount > 0) {
                                    clutBlue = clut + 2;
                                    do {
                                        register s32 x asm("$12");
                                        register s32 y asm("$13");
                                        register s32 z asm("$14");
                                        gte_lwc2_0_0(vertices);
                                        gte_lwc2_1_4(vertices);
                                        gte_cop2_hazard_slot();
                                        gte_cop2_hazard_slot();
                                        gte_mvmva_rotation_v0_translation_sf12();
                                        vertexIndex++;
                                        vertices++;
                                        gte_mfc2_9(x);
                                        gte_mfc2_10(y);
                                        gte_mfc2_11(z);
                                        scratch->x = x;
                                        scratch->y = y;
                                        scratch->z = z;
                                        if (threshold < scratch->y) {
                                            clut[0] = r;
                                            clutBlue[-1] = g;
                                            clutBlue[0] = b;
                                        }
                                        clutBlue += 4;
                                        clut += 4;
                                    } while (vertexIndex < part->vertex_count);
                                }
                            }
                        }
                    }
                }
            }
            partOffset += sizeof(RenderObjectPart);
            i++;
            matrix += 8;
        } while (i < entity->header->part_count);
    }
    __asm__("" : "=m"(frameReserve));
    Render_UpdateClutTable(entity, 0, D_8009CDDC);
}
