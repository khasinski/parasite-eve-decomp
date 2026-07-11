typedef unsigned char u8;
typedef signed char s8;
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
} RenderObjectHeader;

typedef struct RenderObjectEntity {
    RenderObjectHeader *header;
    RenderObjectPart *parts;
    RenderVec3s *vertices;
    u32 *vertex_colours;
    u8 pad10[8];
    RenderVec3s *bounds_vertices;
    u8 pad1C[0x68];
    s32 *matrices;
    u8 shade;
    u8 pad89[0x31];
    s16 draw_count;
} RenderObjectEntity;

extern RenderVec3s D_80091A58[];
extern u32 D_8009CDA0;
extern u32 D_800A6360[];
extern u32 D_800B1638[];

static void Render_DrawObjectLoadRotMatrix(s32 *matrix) {
    int w0;
    int w1;
    int w2;
    int w3;
    int w4;

    w0 = matrix[0];
    w1 = matrix[1];
    w2 = matrix[2];
    w3 = matrix[3];
    w4 = matrix[4];

    asm volatile("ctc2 %0,$0" : : "r"(w0));
    asm volatile("ctc2 %0,$1" : : "r"(w1));
    asm volatile("ctc2 %0,$2" : : "r"(w2));
    asm volatile("ctc2 %0,$3" : : "r"(w3));
    asm volatile("ctc2 %0,$4" : : "r"(w4));
}

static void Render_DrawObjectLoadTrans(s32 *matrix) {
    int w5;
    int w6;
    int w7;

    w5 = matrix[5];
    w6 = matrix[6];
    w7 = matrix[7];

    asm volatile("ctc2 %0,$5" : : "r"(w5));
    asm volatile("ctc2 %0,$6" : : "r"(w6));
    asm volatile("ctc2 %0,$7" : : "r"(w7));
}

static void Render_DrawObjectLoadLightMatrix(s32 *matrix) {
    int w0;
    int w1;
    int w2;
    int w3;
    int w4;

    w0 = matrix[0];
    w1 = matrix[1];
    w2 = matrix[2];
    w3 = matrix[3];
    w4 = matrix[4];

    asm volatile("ctc2 %0,$8" : : "r"(w0));
    asm volatile("ctc2 %0,$9" : : "r"(w1));
    asm volatile("ctc2 %0,$10" : : "r"(w2));
    asm volatile("ctc2 %0,$11" : : "r"(w3));
    asm volatile("ctc2 %0,$12" : : "r"(w4));
}

static void Render_DrawObjectSetFarColour(u8 shade) {
    int value;

    value = shade << 4;
    asm volatile("ctc2 %0,$13" : : "r"(value));
    asm volatile("ctc2 %0,$14" : : "r"(value));
    asm volatile("ctc2 %0,$15" : : "r"(value));
}

static void Render_DrawObjectTransformAxis(u16 *src, s16 *dst) {
    int x;
    int y;
    int z;

    asm volatile("mtc2 %0,$9" : : "r"(src[0]));
    asm volatile("mtc2 %0,$10" : : "r"(src[3]));
    asm volatile("mtc2 %0,$11" : : "r"(src[6]));
    asm volatile("nop");
    asm volatile(".word 0x4A49E012");
    asm volatile("mfc2 %0,$9" : "=r"(x));
    asm volatile("mfc2 %0,$10" : "=r"(y));
    asm volatile("mfc2 %0,$11" : "=r"(z));

    dst[0] = x;
    dst[3] = y;
    dst[6] = z;
}

static void Render_DrawObjectBuildPartLightMatrix(s32 *view_matrix, s32 *part_matrix, s32 *out_matrix) {
    Render_DrawObjectLoadRotMatrix(view_matrix);
    Render_DrawObjectTransformAxis((u16 *)part_matrix, (s16 *)out_matrix);
    Render_DrawObjectTransformAxis((u16 *)part_matrix + 1, (s16 *)out_matrix + 1);
    Render_DrawObjectTransformAxis((u16 *)part_matrix + 2, (s16 *)out_matrix + 2);
}

static void Render_DrawObjectShadeTriple(RenderVec3s *normal_a, RenderVec3s *normal_b, RenderVec3s *normal_c,
                                         u32 colour, u32 *out) {
    asm volatile("lwc2 $0,0(%0)" : : "r"(normal_a));
    asm volatile("lwc2 $1,4(%0)" : : "r"(normal_a));
    asm volatile("lwc2 $2,0(%0)" : : "r"(normal_b));
    asm volatile("lwc2 $3,4(%0)" : : "r"(normal_b));
    asm volatile("lwc2 $4,0(%0)" : : "r"(normal_c));
    asm volatile("lwc2 $5,4(%0)" : : "r"(normal_c));
    asm volatile("mtc2 %0,$6" : : "r"(colour));
    asm volatile("nop");
    asm volatile(".word 0x4B18043F");
    asm volatile("swc2 $20,0(%0)" : : "r"(out) : "memory");
    asm volatile("swc2 $21,4(%0)" : : "r"(out) : "memory");
    asm volatile("swc2 $22,8(%0)" : : "r"(out) : "memory");
}

static void Render_DrawObjectShadePart(RenderObjectEntity *entity, RenderObjectPart *part, u32 base_colour) {
    RenderVec3s *vertices;
    u32 *main_colours;
    u32 *alt_colours;
    u32 *vertex_colours;
    int i;

    vertices = entity->vertices + part->vertex_start;
    main_colours = D_800B1638 + part->vertex_start;
    alt_colours = D_800A6360 + part->vertex_start;
    vertex_colours = entity->vertex_colours + part->vertex_start;

    for (i = 0; i < part->vertex_count; i += 3, vertices += 3, main_colours += 3, alt_colours += 3, vertex_colours += 3) {
        RenderVec3s *normal_a;
        RenderVec3s *normal_b;
        RenderVec3s *normal_c;
        u32 override_colour;

        normal_a = D_80091A58 + vertices[0].pad;
        normal_b = D_80091A58 + vertices[1].pad;
        normal_c = D_80091A58 + vertices[2].pad;

        Render_DrawObjectShadeTriple(normal_a, normal_b, normal_c, base_colour, main_colours);

        override_colour = 0;
        if (((u8 *)&vertex_colours[0])[3] != 0) {
            override_colour = vertex_colours[0];
        } else if (((u8 *)&vertex_colours[1])[3] != 0) {
            override_colour = vertex_colours[1];
        } else if (((u8 *)&vertex_colours[2])[3] != 0) {
            override_colour = vertex_colours[2];
        }

        if (override_colour != 0) {
            Render_DrawObjectShadeTriple(normal_a, normal_b, normal_c, override_colour, alt_colours);
        }
    }
}

void Render_DrawObject(RenderObjectEntity *entity, s32 *view_matrix) {
    s32 light_matrix[8];
    RenderObjectPart *part;
    int i;
    u32 base_colour;

    if (entity->header == 0) {
        return;
    }
    if (entity->draw_count == 0) {
        return;
    }

    base_colour = D_8009CDA0;
    Render_DrawObjectSetFarColour(entity->shade);

    part = entity->parts;
    for (i = 0; i < entity->header->part_count; i++, part++) {
        if (part->pad4 == 1) {
            Render_DrawObjectBuildPartLightMatrix(view_matrix, entity->matrices + i * 8, light_matrix);
            Render_DrawObjectLoadLightMatrix(light_matrix);
            Render_DrawObjectShadePart(entity, part, base_colour);
        }
    }
}
