/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
extern struct { char _[16]; } D_800A6360_o __asm__("D_800A6360");
extern struct { char _[16]; } D_800B1638_o __asm__("g_RenderClutLookupTable");

void Render_UpdateClutTable(void *arg0, s16 arg1, s16 arg2) {
    u8 *hdr;
    u32 *clutTbl;
    u32 *tpageTbl;
    u8 *src;
    u8 *prim;
    u8 *p;
    s32 i;
    u8 cmd;

    hdr = *(u8 **)arg0;
    if ((hdr != NULL) && (*(s16 *)((u8 *)arg0 + 0xBA) != 0)) {
        clutTbl = (u32 *)&D_800B1638_o;
        tpageTbl = (u32 *)&D_800A6360_o;
        src = *(u8 **)((u8 *)arg0 + 0x10);
        prim = *(u8 **)((u8 *)arg0 + 0x54);
        for (i = 0; i < (s32)*(u16 *)(*(u8 **)arg0 + 8); src += 0xC) {
            p = prim + (arg2 * 0x34);
            if (((*(u32 *)p & 0xFFFFFF) != 0) || (arg1 != 0)) {
                cmd = p[7];
                *(u32 *)(p + 4) = clutTbl[*(u16 *)(src + 4)];
                *(u32 *)(p + 0x10) = clutTbl[*(u16 *)(src + 6)];
                *(u32 *)(p + 0x1C) = clutTbl[*(u16 *)(src + 8)];
                *(u32 *)(p + 0x28) = clutTbl[*(u16 *)(src + 0xA)];
                p[7] = cmd;
            }
            prim += 0x68;
            i++;
        }
        for (i = 0; i < (s32)*(u16 *)(*(u8 **)arg0 + 0xA); src += 0xC) {
            p = prim + (arg2 * 0x28);
            if (((*(u32 *)p & 0xFFFFFF) != 0) || (arg1 != 0)) {
                cmd = p[7];
                *(u32 *)(p + 4) = clutTbl[*(u16 *)(src + 4)];
                *(u32 *)(p + 0x10) = clutTbl[*(u16 *)(src + 6)];
                *(u32 *)(p + 0x1C) = clutTbl[*(u16 *)(src + 8)];
                p[7] = cmd;
            }
            prim += 0x50;
            i++;
        }
        for (i = 0; i < (s32)*(u16 *)(*(u8 **)arg0 + 0xC); src += 0xC) {
            p = prim + (arg2 * 0x24);
            if (((*(u32 *)p & 0xFFFFFF) != 0) || (arg1 != 0)) {
                cmd = p[7];
                *(u32 *)(p + 4) = tpageTbl[*(u16 *)(src + 4)];
                *(u32 *)(p + 0xC) = tpageTbl[*(u16 *)(src + 6)];
                *(u32 *)(p + 0x14) = tpageTbl[*(u16 *)(src + 8)];
                *(u32 *)(p + 0x1C) = tpageTbl[*(u16 *)(src + 0xA)];
                p[7] = cmd;
            }
            prim += 0x48;
            i++;
        }
        for (i = 0; i < (s32)*(u16 *)(*(u8 **)arg0 + 0xE); src += 0xC) {
            p = prim + (arg2 * 0x1C);
            if (((*(u32 *)p & 0xFFFFFF) != 0) || (arg1 != 0)) {
                cmd = p[7];
                *(u32 *)(p + 4) = tpageTbl[*(u16 *)(src + 4)];
                *(u32 *)(p + 0xC) = tpageTbl[*(u16 *)(src + 6)];
                *(u32 *)(p + 0x14) = tpageTbl[*(u16 *)(src + 8)];
                p[7] = cmd;
            }
            prim += 0x38;
            i++;
        }
    }
}
