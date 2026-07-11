typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

extern u8 D_8009CD98[];
extern s16 D_8009CD9A;

void Render_SetupBoneTransforms(u8 *actor, s32 *view_matrix);

static void Render_SkinnedLoadRotMatrix(s32 *matrix) {
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

static void Render_SkinnedLoadTrans(s32 *matrix) {
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

static void Render_SkinnedLoadFullMatrix(s32 *matrix) {
    Render_SkinnedLoadRotMatrix(matrix);
    Render_SkinnedLoadTrans(matrix);
}

static void Render_SkinnedTransformVec(void *src, void *dst) {
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

    S16_AT(dst, 0) = x;
    S16_AT(dst, 2) = y;
    S16_AT(dst, 4) = z;
}

static void Render_SkinnedTransformAxis(u16 *src, s16 *dst) {
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

static void Render_SkinnedTransformTranslation(s32 *bone_matrix, s32 *dst_matrix) {
    s32 *out;

    out = dst_matrix + 5;
    asm volatile("lwc2 $0,20(%0)\n\t"
                 "lwc2 $1,24(%0)\n\t"
                 "nop\n\t"
                 "nop\n\t"
                 ".word 0x4A480012\n\t"
                 "swc2 $25,0(%1)\n\t"
                 "swc2 $26,4(%1)\n\t"
                 "swc2 $27,8(%1)"
                 :
                 : "r"(bone_matrix), "r"(out)
                 : "memory");
}

static void Render_SkinnedBuildMatrix(s32 *view_matrix, s32 *bone_matrix, s32 *out_matrix) {
    Render_SkinnedLoadRotMatrix(view_matrix);
    Render_SkinnedTransformAxis((u16 *)bone_matrix, (s16 *)out_matrix);
    Render_SkinnedTransformAxis((u16 *)bone_matrix + 1, (s16 *)out_matrix + 1);
    Render_SkinnedTransformAxis((u16 *)bone_matrix + 2, (s16 *)out_matrix + 2);
    Render_SkinnedLoadTrans(view_matrix);
    Render_SkinnedTransformTranslation(bone_matrix, out_matrix);
}

static u32 Render_SkinnedProject(void *src) {
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

void Render_TransformSkinnedVertices(u8 *actor, s32 *view_matrix) {
    s32 scratch_matrix[8];
    u16 scratch_vec[4];
    u8 *header;
    u8 *matrix_base;
    s32 *bone_matrix;
    u8 *point;
    int bone_index;
    int header_bone_index;

    header = PTR_AT(actor, 0);
    if (header == 0) {
        Render_SetupBoneTransforms(actor, view_matrix);
        return;
    }

    matrix_base = PTR_AT(actor, 0x84);
    header_bone_index = U16_AT(header, 0x12);
    bone_matrix = (s32 *)(matrix_base + header_bone_index * 0x20);
    Render_SkinnedLoadFullMatrix(bone_matrix);

    D_8009CD9A = U16_AT(header, 0x10);
    Render_SkinnedTransformVec(D_8009CD98, actor + 0x68);

    Render_SkinnedLoadFullMatrix((s32 *)matrix_base);
    point = PTR_AT(actor, 0x14);
    scratch_vec[0] = U16_AT(point, 0) + U16_AT(actor, 0x74);
    scratch_vec[1] = U16_AT(point, 2) + U16_AT(actor, 0x76);
    scratch_vec[2] = U16_AT(point, 4) + U16_AT(actor, 0x78);
    Render_SkinnedTransformVec(scratch_vec, actor + 0x74);

    bone_index = S16_AT(actor, 0x32);
    bone_matrix = (s32 *)(matrix_base + bone_index * 0x20);
    Render_SkinnedLoadFullMatrix(bone_matrix);
    point = PTR_AT(actor, 0x18) + bone_index * 0x10;
    Render_SkinnedTransformVec(point, actor + 0xB4);

    Render_SkinnedBuildMatrix(view_matrix, (s32 *)matrix_base, scratch_matrix);
    Render_SkinnedLoadFullMatrix(scratch_matrix);
    U32_AT(actor, 0x5C) = Render_SkinnedProject(PTR_AT(actor, 0x1C));

    Render_SkinnedBuildMatrix(view_matrix, bone_matrix, scratch_matrix);
    Render_SkinnedLoadFullMatrix(scratch_matrix);
    U32_AT(actor, 0x64) = Render_SkinnedProject(point);
}
