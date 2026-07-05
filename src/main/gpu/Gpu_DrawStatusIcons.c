/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern char *g_ActiveActor[];
extern u8 g_MenuActiveMode;
extern int g_ActiveDrawSlot[];
extern char D_8009E968[];
extern char D_8009EA48[];
extern char D_8009EA64[];
extern char D_8009EA80[];
extern char D_8009EA9C[];
extern char D_8009EAB8[];
extern char *g_OtBufferTable[];

void AddPrim(unsigned int *ot, unsigned int *prim);

#define INDEX_STRIDE(i) ((((((i) << 1) + (i)) << 3) - (i)) << 2) - (i)

#define EMIT_CODED(code_expr) do { \
    int packet_code; \
    register char *base asm("$6"); \
    register int code_off asm("$3"); \
    int idx; \
    int idx_off; \
    register void *prim asm("$5"); \
    register void *packet asm("$2"); \
    void *ot; \
    packet_code = (code_expr); \
    base = D_8009E968; \
    code_off = ((packet_code << 3) - packet_code) << 2; \
    prim = base - 8; \
    asm volatile("" : "=r"(prim) : "0"(prim)); \
    idx = g_ActiveDrawSlot[0]; \
    prim = (char *)(code_off + (int)prim); \
    idx_off = INDEX_STRIDE(idx) << 2; \
    prim = (char *)(idx_off + (int)prim); \
    packet = (char *)(idx_off + code_off + (int)base); \
    idx <<= 2; \
    *(u16 *)((char *)packet + 8) = s0; \
    *(u16 *)((char *)packet + 0xA) = s2; \
    ot = *(void **)((char *)g_OtBufferTable + idx); \
    s0 += s3; \
    AddPrim((char *)ot + 0x1C, prim); \
} while (0)

#define EMIT_BASE(base_sym) do { \
    char *base; \
    int idx; \
    register void *prim asm("$5"); \
    register void *packet asm("$2"); \
    base = (base_sym); \
    idx = g_ActiveDrawSlot[0]; \
    prim = base - 8; \
    packet = (char *)(INDEX_STRIDE(idx) << 2); \
    prim = (char *)packet + (int)prim; \
    packet = (char *)packet + (int)base; \
    idx <<= 2; \
    *(u16 *)((char *)packet + 8) = s0; \
    *(u16 *)((char *)packet + 0xA) = s2; \
    s0 += s3; \
    AddPrim(*(char **)((char *)g_OtBufferTable + idx) + 0x1C, prim); \
} while (0)

#define EMIT_BASE_LAST(base_sym) do { \
    int idx; \
    register char *base asm("$6"); \
    int idx_off; \
    register void *packet asm("$3"); \
    idx = g_ActiveDrawSlot[0]; \
    base = (base_sym); \
    idx_off = INDEX_STRIDE(idx) << 2; \
    packet = (char *)(idx_off + (int)base); \
    idx <<= 2; \
    base = base - 8; \
    *(u16 *)((char *)packet + 8) = s0; \
    *(u16 *)((char *)packet + 0xA) = s2; \
    AddPrim(*(char **)((char *)g_OtBufferTable + idx) + 0x1C, (char *)idx_off + (int)base); \
} while (0)

void Gpu_DrawStatusIcons(void) {
    char *s1;
    int s0;
    register int s2 asm("$18");
    register int s3 asm("$19");
    char *s4;
    u8 mode;
    int bits;

    s1 = g_ActiveActor[0];
    mode = g_MenuActiveMode;
    s0 = 0xF;
    s4 = s1 + 0x4C;
    if ((u32)(mode - 1) < 2) {
        s0 = 0x121;
    }
    if (mode < 2) {
        s2 = 0xC1;
    } else {
        s2 = 0xF;
    }
    if ((short)s0 == 0x121) {
        s3 = -0x10;
    } else {
        s3 = 0x10;
    }

    {
        int bits0;
        bits0 = *(int *)(s1 + 0x4C) & 0xC;
        if (bits0 != 0) {
            register int code asm("$5");
            code = bits0 ^ 0xC;
            code = (u32)code < 1;
            code <<= 2;
            EMIT_CODED(code);
        }
    }

    {
        int tmp;
        tmp = *(int *)(s1 + 0x4C);
        bits = tmp & 0x30;
    }
    if (bits != 0) {
        register int code asm("$5");
        code = 1;
        if (bits == 0x30) {
            code = 5;
        }
        EMIT_CODED(code);
    }

    {
        int tmp;
        tmp = *(int *)s4;
        bits = tmp & 0xC0;
    }
    if (bits != 0) {
        register int code asm("$5");
        code = 2;
        if (bits == 0xC0) {
            code = 6;
        }
        EMIT_CODED(code);
    }

    {
        int tmp;
        tmp = *(int *)s4;
        bits = tmp & 3;
    }
    if (bits != 0) {
        register int code asm("$5");
        code = 3;
        if (bits == 3) {
            code = 7;
        }
        EMIT_CODED(code);
    }

    if (*(int *)s4 & 0x1000) {
        EMIT_BASE(D_8009EA48);
    }
    if (*(int *)s4 & 0x100) {
        EMIT_BASE(D_8009EA64);
    }
    if (*(int *)s4 & 0x400) {
        EMIT_BASE(D_8009EA80);
    }
    if (*(int *)s4 & 0x800) {
        EMIT_BASE(D_8009EA9C);
    }
    if (*(int *)s4 & 0x200) {
        EMIT_BASE_LAST(D_8009EAB8);
    }
}
