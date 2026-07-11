typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))
#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define S32_AT(ptr, off) (*(s32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

extern u8 *D_8009D2F0;
extern s32 D_8009CDDC;
extern s32 D_800BCFA4[];
extern s32 D_800CEA40[];

int RotMatrix(void *rot, void *matrix);
void Render_InitRoomPrimState(u8 *actor);
void Anim_BuildRotationMatrices(u8 *actor, u8 *matrix_list, int arg2, int arg3);
void Render_TransformVertices(u8 *actor, s32 *view_matrix);
void Render_TransformSkinnedVertices(u8 *actor, s32 *view_matrix);
void Render_DrawObject(u8 *actor, s32 *prim_state);
void Render_UpdateClutTable(u8 *actor, int arg1, int draw_slot);

static void Task_SetGteLoadRotMatrix(s32 *matrix) {
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

static void Task_SetGteLoadTrans(s32 *matrix) {
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

static void Task_SetGteTransformAxis(u16 *src, s16 *dst) {
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

static void Task_SetGteTransformVectorWords(s32 *src, s32 *dst) {
    asm volatile("lwc2 $0,0(%0)\n\t"
                 "lwc2 $1,4(%0)\n\t"
                 "nop\n\t"
                 "nop\n\t"
                 ".word 0x4A480012\n\t"
                 "swc2 $9,0(%1)\n\t"
                 "swc2 $10,4(%1)\n\t"
                 "swc2 $11,8(%1)"
                 :
                 : "r"(src), "r"(dst)
                 : "memory");
}

static void Task_SetGteBuildScaledMatrix(u8 *actor) {
    s32 zero_matrix_tail[3];
    s16 scale_matrix[16];
    s32 *matrix;
    u16 scale;

    matrix = (s32 *)(actor + 0x1E8);
    scale = U16_AT(actor, 0x26);

    scale_matrix[0] = scale;
    scale_matrix[1] = 0;
    scale_matrix[2] = 0;
    scale_matrix[3] = 0;
    scale_matrix[4] = scale;
    scale_matrix[5] = 0;
    scale_matrix[6] = 0;
    scale_matrix[7] = 0;
    scale_matrix[8] = scale;
    zero_matrix_tail[0] = 0;
    zero_matrix_tail[1] = 0;
    zero_matrix_tail[2] = 0;

    Task_SetGteLoadRotMatrix(matrix);
    Task_SetGteTransformAxis((u16 *)scale_matrix, (s16 *)matrix);
    Task_SetGteTransformAxis((u16 *)scale_matrix + 1, (s16 *)matrix + 1);
    Task_SetGteTransformAxis((u16 *)scale_matrix + 2, (s16 *)matrix + 2);
    Task_SetGteLoadTrans(matrix);
    Task_SetGteTransformVectorWords(zero_matrix_tail, (s32 *)(actor + 0x1FC));
}

static void Task_SetGteCopyPrimColorBytes(u8 *task, u8 *actor) {
    U8_AT(actor, 0x23C) = U32_AT(PTR_AT(task, 0), 0);
    U8_AT(actor, 0x23D) = U32_AT(PTR_AT(task, 4), 0);
    U8_AT(actor, 0x23E) = U32_AT(PTR_AT(task, 8), 0);
}

static void Task_SetGteQueueActor(u8 *actor, u32 flags) {
    U32_AT(actor, 0x98) = flags | 0x08000000;
    asm volatile("lw $3,0x90($gp)\n\t"
                 "lw $4,0x590($gp)\n\t"
                 "addiu $3,$3,-20\n\t"
                 "sw $3,0x90($gp)\n\t"
                 "li $3,1\n\t"
                 "sw $3,16($4)"
                 :
                 :
                 : "$3", "$4", "memory");
}

int Task_SetGteMatrix(u8 *task) {
    u8 *actor;
    u32 flags;

    actor = D_8009D2F0;

    S32_AT(actor, 0x1FC) = S16_AT(actor, 0x2A);
    S32_AT(actor, 0x200) = S16_AT(actor, 0x2E);
    S32_AT(actor, 0x204) = S16_AT(actor, 0x32);
    U16_AT(actor, 0x1E0) = U16_AT(actor, 0x38);
    U16_AT(actor, 0x1E2) = U16_AT(actor, 0x3A);
    U16_AT(actor, 0x1E4) = U16_AT(actor, 0x3C);
    RotMatrix(actor + 0x1E0, actor + 0x1E8);

    Task_SetGteBuildScaledMatrix(actor);
    Task_SetGteCopyPrimColorBytes(task, actor);

    Render_InitRoomPrimState(actor + 0x1B4);
    Anim_BuildRotationMatrices(actor + 0x1B4, PTR_AT(actor, 0x1B0), 0, 1);
    Render_TransformVertices(actor + 0x1B4, (s32 *)PTR_AT(actor, 0x1B0));
    Render_TransformSkinnedVertices(actor + 0x1B4, D_800BCFA4);
    Render_DrawObject(actor + 0x1B4, D_800CEA40);
    Render_UpdateClutTable(actor + 0x1B4, 1, D_8009CDDC);

    flags = U32_AT(actor, 0x98);
    if (flags & 0x10000000) {
        D_8009CDDC ^= 1;
        Render_DrawObject(actor + 0x1B4, D_800CEA40);
        Render_UpdateClutTable(actor + 0x1B4, 1, D_8009CDDC);
        D_8009CDDC ^= 1;
        return 1;
    }

    if (!(flags & 0x08000000)) {
        Task_SetGteQueueActor(actor, flags);
        return 1;
    }

    U32_AT(actor, 0x98) = flags & ~0x08000000;
    return 1;
}
