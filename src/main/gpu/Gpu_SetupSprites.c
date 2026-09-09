#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern u8 g_BattleSpritePrimCountdown;
extern char D_8009E974[];
extern char D_8009E975[];
extern char D_8009E976[];
extern char D_8009EC40[];
extern char D_8009EC4E[];

void Gpu_SetupSprites(void *arg0, int arg1, int arg2) {
    void *obj;
    int i;
    char *dst_base;
    int src_base;

    obj = arg0;
    g_BattleSpritePrimCountdown = 0x1E;
    i = 0;
    dst_base = D_8009EC40;
    arg2 = (u8)arg2;
    src_base = ((arg2 << 3) - arg2) << 2;

    do {
        int i_u8;
        int dst_offset;
        register int src_offset asm("$3");
        char *dst;
        u16 val;

        i_u8 = (u8)i;
        dst_offset = ((i_u8 << 3) - i_u8) << 2;
        asm volatile("" : "=r"(dst_offset) : "0"(dst_offset));
        src_offset = (((((i_u8 * 3) << 3) - i_u8) << 2) - i_u8) << 2;
        src_offset = src_base + src_offset;
        dst = (char *)(dst_offset + (int)dst_base);

        dst[0xC] = *(u8 *)(D_8009E974 + src_offset);
        dst[0xD] = *(u8 *)(D_8009E975 + src_offset);
        *(u16 *)(dst + 0x8) = *(u16 *)((char *)obj + 0x210) - 8;
        *(u16 *)(dst + 0xA) = *(u16 *)((char *)obj + 0x212) - 0x10;
        val = *(u16 *)(D_8009E976 + src_offset);
        i++;
        *(u16 *)(D_8009EC4E + dst_offset) = val;
    } while ((u8)i < 2);
}

#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_ActiveDrawSlot[];
extern u8 g_BattleSpritePrimCountdown;
extern char D_8009EC38[];
extern char D_8009EC4A[];
extern char *g_OtBufferTable[];

void AddPrim(unsigned int *ot, unsigned int *prim);

void Gpu_QueuePrimitive(void) {
    int index;
    int prim_offset;
    int ot_offset;
    void *prim;
    char *ot;

    index = g_ActiveDrawSlot[0];
    prim_offset = ((index << 3) - index) << 2;
    ot_offset = index << 2;
    *(u16 *)(D_8009EC4A + prim_offset) = *(u16 *)(D_8009EC4A + prim_offset) - 2;
    asm volatile("" ::: "memory");
    prim = D_8009EC38 + prim_offset;
    ot = *(char **)((char *)g_OtBufferTable + ot_offset);
    AddPrim(ot + 0x1C, prim);
    g_BattleSpritePrimCountdown--;
}

#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

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
     \
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
