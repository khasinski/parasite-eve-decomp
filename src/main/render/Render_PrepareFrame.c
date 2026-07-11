typedef unsigned char u8;
typedef signed short s16;
typedef unsigned int u32;
typedef int s32;

#define S16_AT(ptr, off) (*(s16 *)((u8 *)(ptr) + (off)))

extern u8 g_EntityRenderScratch[];
extern void *D_800B0E40;
extern u8 *D_8009D254;
extern s32 *D_800BCFA4;
extern s16 D_800BCFB4;
extern s16 D_800BCFB6;

int Menu_InitGlobals(void *packet_dst, void *scratch);
int Geo_RenderMeshList(void *mesh, void *out);
void SetGeomScreen(s32 value);
void func_800655D4(void);

static void Render_PrepareFrameSetScreenOffset(s32 x, s32 y) {
    x <<= 16;
    y <<= 16;
    asm volatile("ctc2 %0,$24" : : "r"(x));
    asm volatile("ctc2 %0,$25" : : "r"(y));
}

static void Render_PrepareFrameLoadMatrix(s32 *matrix) {
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

static u32 Render_PrepareFrameProject(void *src) {
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

s32 Render_PrepareFrame(void) {
    s16 point[4];
    s32 geo_scratch;
    u32 sxy;
    u8 *actor;

    Menu_InitGlobals(g_EntityRenderScratch, g_EntityRenderScratch + 0x20);
    if (Geo_RenderMeshList(D_800B0E40, &geo_scratch) != 0) {
        return -2;
    }

    SetGeomScreen(*(s32 *)(g_EntityRenderScratch + 0x20));
    func_800655D4();

    actor = D_8009D254;
    if (actor != 0) {
        point[0] = S16_AT(actor, 0x2A);
        point[1] = S16_AT(actor, 0x2E);
        point[2] = S16_AT(actor, 0x32);
    } else {
        point[0] = 0;
        point[1] = 0;
        point[2] = 0;
    }
    point[3] = 0;

    Render_PrepareFrameSetScreenOffset(0xA0, 0x70);
    Render_PrepareFrameLoadMatrix(D_800BCFA4);
    sxy = Render_PrepareFrameProject(point);
    D_800BCFB4 = sxy;
    D_800BCFB6 = sxy >> 16;
    return 0;
}
