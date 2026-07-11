typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

typedef struct RenderVec3s {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
} RenderVec3s;

typedef struct RenderObjectPart {
    u16 vertex_start;
    u16 vertex_count;
    u8 pad4;
    u8 pad5[7];
} RenderObjectPart;

typedef struct RenderObjectHeader {
    u8 pad0[2];
    u8 part_count;
    u8 pad3[0x17];
    u16 visible_part_count;
} RenderObjectHeader;

typedef struct RenderObjectEntity {
    RenderObjectHeader *header;
    RenderObjectPart *parts;
    RenderVec3s *vertices;
    u8 pad0C[0x0C];
    RenderVec3s *bounds_vertices;
    u8 pad1C[0x68];
    s32 *matrices;
    u8 pad88[0x14];
    u16 flags9C;
    u8 variant_visible;
    u8 pad9F[0x1B];
    s16 draw_count;
} RenderObjectEntity;

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

    asm volatile("lwc2 $0,0(%0)" : : "r"(src));
    asm volatile("lwc2 $1,4(%0)" : : "r"(src));
    asm volatile("nop");
    asm volatile(".word 0x4A480012");
    asm volatile("mfc2 %0,$9" : "=r"(x));
    asm volatile("mfc2 %0,$10" : "=r"(y));
    asm volatile("mfc2 %0,$11" : "=r"(z));

    dst->x = x;
    dst->y = y;
    dst->z = z;
}

static u32 Render_DrawObjectVariantProject(RenderVec3s *src) {
    u32 sxy;

    asm volatile("lwc2 $0,0(%1)\n\t"
                 "lwc2 $1,4(%1)\n\t"
                 "nop\n\t"
                 "nop\n\t"
                 ".word 0x4A180001\n\t"
                 "swc2 $14,0(%0)"
                 :
                 : "r"(&sxy), "r"(src)
                 : "memory");
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
        if (part->pad4 == 1) {
            visible_parts += Render_DrawObjectVariantColourPart(entity, part, matrix, part_index, threshold);
        }
    }

    if (visible_parts == entity->header->visible_part_count || (entity->flags9C & 0x200)) {
        entity->variant_visible = 0;
    } else {
        entity->variant_visible = 1;
    }
}
