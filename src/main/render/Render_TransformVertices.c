typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

#define S8_AT(ptr, off) (*(s8 *)((u8 *)(ptr) + (off)))
#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

void Render_TransformSkinnedVertices(u8 *actor, s32 *view_matrix);
void Render_TransformMorphVertices(u8 *actor, s32 *view_matrix);

static void Render_XformLoadRotMatrix(s32 *matrix) {
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

static void Render_XformLoadTrans(s32 *matrix) {
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

static void Render_XformLoadFullMatrix(s32 *matrix) {
    Render_XformLoadRotMatrix(matrix);
    Render_XformLoadTrans(matrix);
}

static void Render_XformStoreFullMatrix(s32 *matrix) {
    int w0;
    int w1;
    int w2;
    int w3;
    int w4;
    int w5;
    int w6;
    int w7;

    asm volatile("cfc2 %0,$0" : "=r"(w0));
    asm volatile("cfc2 %0,$1" : "=r"(w1));
    asm volatile("cfc2 %0,$2" : "=r"(w2));
    asm volatile("cfc2 %0,$3" : "=r"(w3));
    asm volatile("cfc2 %0,$4" : "=r"(w4));
    asm volatile("cfc2 %0,$5" : "=r"(w5));
    asm volatile("cfc2 %0,$6" : "=r"(w6));
    asm volatile("cfc2 %0,$7" : "=r"(w7));

    matrix[0] = w0;
    matrix[1] = w1;
    matrix[2] = w2;
    matrix[3] = w3;
    matrix[4] = w4;
    matrix[5] = w5;
    matrix[6] = w6;
    matrix[7] = w7;
}

static void Render_XformTransformAxis(u16 *src, s16 *dst) {
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

static void Render_XformTransformTranslation(s32 *src, s32 *dst) {
    s32 *out;

    out = dst + 5;
    asm volatile("lwc2 $0,20(%0)\n\t"
                 "lwc2 $1,24(%0)\n\t"
                 "nop\n\t"
                 "nop\n\t"
                 ".word 0x4A480012\n\t"
                 "swc2 $25,0(%1)\n\t"
                 "swc2 $26,4(%1)\n\t"
                 "swc2 $27,8(%1)"
                 :
                 : "r"(src), "r"(out)
                 : "memory");
}

static void Render_XformTransformOrigin(s32 *dst) {
    s32 zero_vec[2];
    s32 *out;

    zero_vec[0] = 0;
    zero_vec[1] = 0;
    out = dst + 5;
    asm volatile("lwc2 $0,0(%0)\n\t"
                 "lwc2 $1,4(%0)\n\t"
                 "nop\n\t"
                 "nop\n\t"
                 ".word 0x4A480012\n\t"
                 "swc2 $25,0(%1)\n\t"
                 "swc2 $26,4(%1)\n\t"
                 "swc2 $27,8(%1)"
                 :
                 : "r"(zero_vec), "r"(out)
                 : "memory");
}

static void Render_XformBuildChildMatrix(s32 *src, s32 *dst, int include_translation) {
    Render_XformTransformAxis((u16 *)src, (s16 *)dst);
    Render_XformTransformAxis((u16 *)src + 1, (s16 *)dst + 1);
    Render_XformTransformAxis((u16 *)src + 2, (s16 *)dst + 2);

    if (include_translation != 0) {
        Render_XformTransformTranslation(src, dst);
    } else {
        Render_XformTransformOrigin(dst);
    }
}

static void Render_XformTransformVector(void *src, void *dst) {
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

static void Render_XformCopyMatrixFromActor(u8 *actor) {
    u8 *matrix;

    matrix = PTR_AT(actor, 0x84);
    U16_AT(matrix, 0x20) = U16_AT(actor, 0x34);
    U16_AT(matrix, 0x22) = U16_AT(actor, 0x36);
    U16_AT(matrix, 0x24) = U16_AT(actor, 0x38);
    U16_AT(matrix, 0x26) = U16_AT(actor, 0x3A);
    U16_AT(matrix, 0x28) = U16_AT(actor, 0x3C);
    U16_AT(matrix, 0x2A) = U16_AT(actor, 0x3E);
    U16_AT(matrix, 0x2C) = U16_AT(actor, 0x40);
    U16_AT(matrix, 0x2E) = U16_AT(actor, 0x42);
    U16_AT(matrix, 0x30) = U16_AT(actor, 0x44);
    U32_AT(matrix, 0x34) = U32_AT(actor, 0x48);
    U32_AT(matrix, 0x38) = U32_AT(actor, 0x4C);
    U32_AT(matrix, 0x3C) = U32_AT(actor, 0x50);
}

static void Render_XformUpdateMode4Matrix(u8 *actor) {
    u8 *parent;
    s32 *parent_matrix;
    s32 *actor_matrix;
    int bone_index;

    parent = PTR_AT(actor, 0x24);
    bone_index = S16_AT(actor, 0x2A);
    parent_matrix = (s32 *)(PTR_AT(parent, 0x84) + bone_index * 0x20);
    actor_matrix = (s32 *)(actor + 0x34);

    Render_XformLoadRotMatrix(parent_matrix);
    Render_XformBuildChildMatrix(actor_matrix, actor_matrix, 1);
}

static void Render_XformTransformVisiblePart(u8 *actor, int part_index, u8 **out_vertices) {
    u8 *part;
    u8 *point;

    part = PTR_AT(actor, 4) + part_index * 0x0C;
    if (part[4] != 1) {
        return;
    }

    point = PTR_AT(actor, 0x18) + part_index * 0x10;
    if (S16_AT(point, 0x0E) < 0) {
        return;
    }

    Render_XformTransformVector(point, *out_vertices);
    *out_vertices += 0x0C;
}

void Render_TransformVertices(u8 *actor) {
    s32 matrix_stack[16][8];
    s32 *stack_top;
    u8 *commands;
    u8 *out_matrix;
    u8 *out_vertices;
    u8 *header;
    s32 *current_matrix;
    s32 *src_matrix;
    int mode;
    int part_count;
    int i;
    int command;

    mode = S16_AT(actor, 0x28);
    if (mode == 1) {
        Render_TransformSkinnedVertices(actor, (s32 *)PTR_AT(actor, 0x24));
        current_matrix = (s32 *)(actor + 0x34);
    } else if (mode == 3) {
        if ((U16_AT(actor, 0x9C) & 0x400) != 0) {
            Render_XformCopyMatrixFromActor(actor);
        } else {
            Render_TransformMorphVertices(actor, (s32 *)PTR_AT(actor, 0x24));
        }
        current_matrix = (s32 *)(actor + 0x34);
    } else {
        current_matrix = (s32 *)(actor + 0x34);
        if (mode == 4) {
            Render_XformUpdateMode4Matrix(actor);
        }
    }

    header = PTR_AT(actor, 0);
    part_count = U16_AT(header, 0x18);
    if (part_count <= 0) {
        return;
    }

    stack_top = matrix_stack;
    commands = PTR_AT(actor, 0x20);
    out_matrix = PTR_AT(actor, 0x84);
    out_vertices = PTR_AT(actor, 0x80);

    Render_XformLoadFullMatrix(current_matrix);
    for (i = 0; i < part_count; i++, commands++, out_matrix += 0x20) {
        command = S8_AT(commands, 0);
        if (command == -1) {
            Render_XformStoreFullMatrix(stack_top);
            stack_top += 8;
            continue;
        }

        if (command == -2) {
            stack_top -= 8;
            Render_XformLoadFullMatrix(stack_top);
            continue;
        }

        src_matrix = (s32 *)(PTR_AT(actor, 0x58) + command * 0x20);
        Render_XformBuildChildMatrix(src_matrix, (s32 *)out_matrix, command == 0);
        Render_XformLoadFullMatrix((s32 *)out_matrix);
        Render_XformTransformVisiblePart(actor, command, &out_vertices);
    }
}
