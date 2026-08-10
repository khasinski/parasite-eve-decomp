#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_object.h"

extern u32 D_800B1640[];

static void Render_DrawObjectVariantLoadMatrix(s32 *matrix) {
    int w0;
    int w1;
    int w2;
    int w3;
    int w4;
    int w5;
    int w6;
    int w7;

    w0 = matrix[0];
    w1 = matrix[1];
    w2 = matrix[2];
    w3 = matrix[3];
    w4 = matrix[4];
    w5 = matrix[5];
    w6 = matrix[6];
    w7 = matrix[7];

    asm volatile("ctc2 %0,$0" : : "r"(w0));
    asm volatile("ctc2 %0,$1" : : "r"(w1));
    asm volatile("ctc2 %0,$2" : : "r"(w2));
    asm volatile("ctc2 %0,$3" : : "r"(w3));
    asm volatile("ctc2 %0,$4" : : "r"(w4));
    asm volatile("ctc2 %0,$5" : : "r"(w5));
    asm volatile("ctc2 %0,$6" : : "r"(w6));
    asm volatile("ctc2 %0,$7" : : "r"(w7));
}

static void Render_DrawObjectVariantTransform(RenderVec3s *src, RenderVec3s *dst) {
    int x;
    int y;
    int z;

    gte_ldv0(src);
    gte_rtv0tr();
    gte_getir1(x);
    gte_getir2(y);
    gte_getir3(z);

    dst->x = x;
    dst->y = y;
    dst->z = z;
}

static u32 Render_DrawObjectVariantProject(RenderVec3s *src) {
    u32 sxy;

    gte_ldv0(src);
    gte_rtps();
    gte_stsxy2(&sxy);
    return sxy;
}

static int Render_DrawObjectVariantPartVisible(RenderObjectEntity *entity, int part_index, int threshold,
                                               RenderVec3s *scratch) {
    RenderVec3s *bounds;
    int y;
    int radius;

    Render_DrawObjectVariantLoadMatrix(entity->matrices + part_index * 8);
    bounds = entity->bounds_vertices + part_index * 2;
    Render_DrawObjectVariantTransform(bounds, scratch);

    y = scratch->y;
    if (threshold < y) {
        return 1;
    }

    radius = bounds[0].pad;
    if (threshold < y - radius) {
        return 1;
    }
    return threshold < y + radius;
}

static int Render_DrawObjectVariantColourPart(RenderObjectEntity *entity, RenderObjectPart *part, s32 *matrix,
                                              int part_index, int threshold) {
    RenderVec3s scratch;
    RenderVec3s *vertices;
    u32 projected_colour;
    u32 *clut;
    int i;
    int changed;

    if (!Render_DrawObjectVariantPartVisible(entity, part_index, threshold, &scratch)) {
        return 0;
    }

    scratch.y = threshold;
    Render_DrawObjectVariantLoadMatrix(matrix);
    projected_colour = Render_DrawObjectVariantProject(&scratch);

    vertices = entity->vertices + part->vertex_start;
    clut = D_800B1640 + part->vertex_start;
    Render_DrawObjectVariantLoadMatrix(entity->matrices + part_index * 8);

    changed = 0;
    for (i = 0; i < part->vertex_count; i++) {
        Render_DrawObjectVariantTransform(vertices, &scratch);
        if (threshold < scratch.y) {
            *clut = projected_colour;
            changed++;
        }
        vertices++;
        clut++;
    }

    return changed;
}

void Render_DrawObjectVariant(RenderObjectEntity *entity, s16 threshold, s32 *matrix) {
    RenderObjectPart *part;
    int part_index;
    int visible_parts;

    if (entity->header == 0) {
        return;
    }
    if (entity->draw_count == 0) {
        return;
    }

    visible_parts = 0;
    part = entity->parts;
    for (part_index = 0; part_index < entity->header->part_count; part_index++, part++) {
        if (part->visible == 1) {
            visible_parts += Render_DrawObjectVariantColourPart(entity, part, matrix, part_index, threshold);
        }
    }

    if (visible_parts == entity->header->visible_part_count || (entity->flags_9C & 0x200)) {
        entity->variant_visible = 0;
    } else {
        entity->variant_visible = 1;
    }
}
