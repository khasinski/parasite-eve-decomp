/* CC1_FLAGS: -fno-schedule-insns */
/* Matching debt: register pins, empty barriers, and a 16-byte local frame
 * reservation reproduce the retail allocation. The reservation is artificial.
 * GTE operations and their explicit nop hazard slots use individual macros. */

#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_object.h"

extern s16 D_8009CDDC;

#define Render_LoadObjectMatrix(matrix)                                                            \
    {                                                                                              \
        gte_ldrotmatrix(matrix); \
        gte_ldtransmatrix(matrix); \
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
    RenderObjectEntity *entity = input;
    u8 r = red;
    u8 g = green;
    u8 b = blue;
    s32 threshold;
    register volatile RenderVec3s *scratch asm("$6") = (RenderVec3s *)0x1F800000;
    RenderObjectPart *part;
    s32 i;
    register s32 partOffset asm("$24");
    int *matrix;
    s32 limitShift;
    s32 count;
    u32 frameReserve[4];
    s32 boundsOffset;
    register s32 clipSum asm("$2");
    RenderVec3s *bounds;
    s32 y;
    register s32 radius asm("$4");
    RenderVec3s *radiusBounds;
    RenderVec3s *vertices;
    u8 *clut;
    u8 *clutBlue;
    register s32 vertexIndex asm("$5");
    if (!entity->header || !entity->draw_count)
        return;
    matrix = (int *)entity->matrices;
    i = 0;
    count = entity->header->part_count;
    if (count > 0) {
        limitShift = limit << 16;
        threshold = limitShift >> 16;
        partOffset = 0;
        do {
            {
                RenderObjectPart *probe =
                    (RenderObjectPart *)(partOffset + (u32)entity->parts);
                if (probe->visible == 1) {
                    Render_LoadObjectMatrix(matrix);
                    boundsOffset = i * 16;
                    bounds = (RenderVec3s *)((u8 *)entity->bounds_vertices + boundsOffset);
                    Render_TransformVertex(bounds, scratch);
                    {
                        RenderObjectPart *parts = entity->parts;
                        u16 yy = (u16)scratch->y;
                        part = (RenderObjectPart *)((u8 *)parts + partOffset);
                        y = (s16)yy;
                    }
                    if (threshold < y ||
                        (radiusBounds =
                             (RenderVec3s *)(boundsOffset + (u32)entity->bounds_vertices),
                         radius = radiusBounds->pad,
                         threshold < y - radius || threshold < (clipSum = y + radius))) {
                        {
                            s32 first = part->vertex_start;
                            RenderVec3s *vertexBase = entity->vertices;
                            s32 vertexOffset = first * 8;
                            vertices = (RenderVec3s *)((u8 *)vertexBase + vertexOffset);
                            first <<= 2;
                            {
                                register u8 *clutBase asm("$2") = (u8 *)D_800B1638;
                                s32 vertexCount = part->vertex_count;
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
    Render_UpdateClutTable(entity, 0, D_8009CDDC);
}
