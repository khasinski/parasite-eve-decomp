typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
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
} RenderObjectHeader;

typedef struct RenderObjectEntity {
    RenderObjectHeader *header;
    RenderObjectPart *parts;
    RenderVec3s *vertices;
    u8 pad0C[0x0C];
    RenderVec3s *bounds_vertices;
    u8 pad1C[0x68];
    s32 *matrices;
    u8 pad88[0x32];
    s16 draw_count;
} RenderObjectEntity;

extern u8 D_800B1638[];
extern s16 D_8009CDDC;

void Render_UpdateClutTable(void *entity, int arg1, s16 draw_slot);

static void Render_LoadObjectMatrix(int *matrix) {
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

static void Render_TransformVertex(RenderVec3s *src, RenderVec3s *dst) {
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

static int Render_ObjectAltPartVisible(RenderObjectEntity *entity, int part_index, int threshold) {
    RenderVec3s scratch;
    RenderVec3s *bounds;
    int y;
    int radius;

    Render_LoadObjectMatrix(entity->matrices + part_index * 8);
    bounds = entity->bounds_vertices + part_index * 2;
    Render_TransformVertex(bounds, &scratch);

    y = scratch.y;
    if (threshold < y) {
        return 1;
    }

    radius = bounds[0].pad;
    if (threshold < y - radius) {
        return 1;
    }
    return threshold < y + radius;
}

static void Render_ObjectAltColourVertices(RenderObjectEntity *entity, RenderObjectPart *part, int threshold,
                                           u8 r, u8 g, u8 b) {
    RenderVec3s scratch;
    RenderVec3s *vertices;
    u8 *clut;
    int i;

    vertices = entity->vertices + part->vertex_start;
    clut = D_800B1638 + part->vertex_start * 4;

    for (i = 0; i < part->vertex_count; i++) {
        Render_TransformVertex(vertices, &scratch);
        if (threshold < scratch.y) {
            clut[0] = r;
            clut[1] = g;
            clut[2] = b;
        }
        vertices++;
        clut += 4;
    }
}

void Render_DrawObjectAlt(RenderObjectEntity *entity, s16 threshold, u8 r, u8 g, u8 b) {
    RenderObjectPart *part;
    int i;

    if (entity->header == 0) {
        return;
    }
    if (entity->draw_count == 0) {
        return;
    }

    part = entity->parts;
    for (i = 0; i < entity->header->part_count; i++, part++) {
        if (part->pad4 == 1 && Render_ObjectAltPartVisible(entity, i, threshold)) {
            Render_ObjectAltColourVertices(entity, part, threshold, r, g, b);
        }
    }

    Render_UpdateClutTable(entity, 0, D_8009CDDC);
}
