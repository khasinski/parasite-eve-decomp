typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

#include "pe1/gte.h"

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
} RenderObjectEntity;

extern u32 D_800A6360[];
extern u32 D_800B1638[];

static void Render_MorphLoadRotMatrix(s32 *matrix) {
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

static void Render_MorphLoadTrans(s32 *matrix) {
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

static void Render_MorphLoadFullMatrix(s32 *matrix) {
    Render_MorphLoadRotMatrix(matrix);
    Render_MorphLoadTrans(matrix);
}

static void Render_MorphTransformAxis(u16 *src, s16 *dst) {
    int x;
    int y;
    int z;

    gte_ldir123(src[0], src[3], src[6]);
    gte_rtir12();
    gte_getir1(x);
    gte_getir2(y);
    gte_getir3(z);

    dst[0] = x;
    dst[3] = y;
    dst[6] = z;
}

static void Render_MorphTransformTranslation(s32 *part_matrix, s32 *dst_matrix) {
    s32 *out;

    out = dst_matrix + 5;
    gte_ldv0(part_matrix + 5);
    gte_rtv0tr_mac();
    gte_stmac(out);
}

static void Render_MorphBuildMatrix(s32 *view_matrix, s32 *part_matrix, s32 *out_matrix) {
    Render_MorphLoadRotMatrix(view_matrix);
    Render_MorphTransformAxis((u16 *)part_matrix, (s16 *)out_matrix);
    Render_MorphTransformAxis((u16 *)part_matrix + 1, (s16 *)out_matrix + 1);
    Render_MorphTransformAxis((u16 *)part_matrix + 2, (s16 *)out_matrix + 2);
    Render_MorphLoadTrans(view_matrix);
    Render_MorphTransformTranslation(part_matrix, out_matrix);
}

static void Render_MorphTransformTriple(RenderVec3s *src, u32 *screen_out, u32 *depth_out) {
    gte_ldv012(src);
    gte_rtpt();
    gte_stsxy012(screen_out);
    gte_stsz123(depth_out);
}

static void Render_MorphTransformPart(RenderObjectEntity *entity, RenderObjectPart *part) {
    RenderVec3s *vertices;
    u32 *screen_out;
    u32 *depth_out;
    int i;

    vertices = entity->vertices + part->vertex_start;
    screen_out = D_800B1638 + part->vertex_start;
    depth_out = D_800A6360 + part->vertex_start;

    for (i = 0; i < part->vertex_count; i += 3, vertices += 3, screen_out += 3, depth_out += 3) {
        Render_MorphTransformTriple(vertices, screen_out, depth_out);
    }
}

void Render_TransformMorphVertices(RenderObjectEntity *entity, s32 *view_matrix) {
    s32 morph_matrix[8];
    RenderObjectPart *part;
    s32 *part_matrix;
    int i;

    if (entity->header->part_count <= 0) {
        return;
    }

    part = entity->parts;
    part_matrix = entity->matrices;
    for (i = 0; i < entity->header->part_count; i++, part++, part_matrix += 8) {
        Render_MorphBuildMatrix(view_matrix, part_matrix, morph_matrix);
        Render_MorphLoadFullMatrix(morph_matrix);

        if (part->pad4 == 1) {
            Render_MorphTransformPart(entity, part);
        }
    }
}
