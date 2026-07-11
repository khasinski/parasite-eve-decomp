typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

static void Render_SetupBoneLoadRotMatrix(s32 *matrix) {
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

static void Render_SetupBoneLoadTrans(s32 *matrix) {
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

static void Render_SetupBoneLoadFullMatrix(s32 *matrix) {
    Render_SetupBoneLoadRotMatrix(matrix);
    Render_SetupBoneLoadTrans(matrix);
}

static void Render_SetupBoneTransformVec(void *src, void *dst) {
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

static void Render_SetupBoneTransformAxis(u16 *src, s16 *dst) {
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

static void Render_SetupBoneTransformTranslation(s32 *bone_matrix, s32 *dst_matrix) {
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

static void Render_SetupBoneBuildMatrix(s32 *view_matrix, s32 *bone_matrix, s32 *out_matrix) {
    Render_SetupBoneLoadRotMatrix(view_matrix);
    Render_SetupBoneTransformAxis((u16 *)bone_matrix, (s16 *)out_matrix);
    Render_SetupBoneTransformAxis((u16 *)bone_matrix + 1, (s16 *)out_matrix + 1);
    Render_SetupBoneTransformAxis((u16 *)bone_matrix + 2, (s16 *)out_matrix + 2);
    Render_SetupBoneLoadTrans(view_matrix);
    Render_SetupBoneTransformTranslation(bone_matrix, out_matrix);
}

static u32 Render_SetupBoneProject(void *src) {
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

void Render_SetupBoneTransforms(u8 *actor, s32 *view_matrix) {
    s32 scratch_matrix[8];
    u8 *render_obj;
    s32 *bone_matrix;
    u8 *bone_point;
    int bone_index;
    u32 sxy;

    render_obj = PTR_AT(actor, 0x24);
    bone_index = S16_AT(actor, 0x32);
    bone_matrix = (s32 *)(PTR_AT(render_obj, 0x84) + bone_index * 0x20);

    Render_SetupBoneLoadFullMatrix(bone_matrix);
    bone_point = PTR_AT(render_obj, 0x18) + bone_index * 0x10;
    Render_SetupBoneTransformVec(bone_point, actor + 0xA0);
    Render_SetupBoneTransformVec(bone_point, actor + 0x68);
    Render_SetupBoneTransformVec(bone_point, actor + 0xB4);

    Render_SetupBoneBuildMatrix(view_matrix, bone_matrix, scratch_matrix);
    Render_SetupBoneLoadFullMatrix(scratch_matrix);

    sxy = Render_SetupBoneProject(actor + 0x2C);
    U16_AT(actor, 0x7C) = U16_AT(render_obj, 0x7C);
    U32_AT(actor, 0x64) = sxy;
    U16_AT(actor, 0x2C) = U16_AT(actor, 0x2C) + U16_AT(actor, 0x70);

    sxy = Render_SetupBoneProject(actor + 0x2C);
    U16_AT(actor, 0x74) = U16_AT(render_obj, 0x74);
    U16_AT(actor, 0x76) = U16_AT(render_obj, 0x76);
    U16_AT(actor, 0x78) = U16_AT(render_obj, 0x78);
    U32_AT(actor, 0x5C) = sxy;
    U16_AT(actor, 0x2C) = U16_AT(actor, 0x2C) - U16_AT(actor, 0x70);
}
