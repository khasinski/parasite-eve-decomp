#include "common.h"
#include "pe1/render_object.h"
#define NULL ((void *)0)
extern u32 D_800A6360[];
extern u32 g_RenderClutLookupTable[];

void Render_UpdateClutTable(RenderObjectEntity *arg0, s16 arg1, s16 arg2) {
    RenderObjectHeader *hdr;
    u32 *clutTbl;
    u32 *tpageTbl;
    RenderPrimitiveDescriptor *src;
    u8 *prim;
    u8 *p;
    s32 i;
    u8 cmd;

    hdr = arg0->header;
    if ((hdr != NULL) && (arg0->draw_count != 0)) {
        clutTbl = g_RenderClutLookupTable;
        tpageTbl = D_800A6360;
        src = arg0->primitive_descriptors;
        prim = arg0->primitive_buffer;
        for (i = 0; i < arg0->header->packet34_count; src++) {
            p = prim + (arg2 * 0x34);
            if (((*(u32 *)p & 0xFFFFFF) != 0) || (arg1 != 0)) {
                cmd = p[7];
                *(u32 *)(p + 4) = clutTbl[src->lookup_indices[0]];
                *(u32 *)(p + 0x10) = clutTbl[src->lookup_indices[1]];
                *(u32 *)(p + 0x1C) = clutTbl[src->lookup_indices[2]];
                *(u32 *)(p + 0x28) = clutTbl[src->lookup_indices[3]];
                p[7] = cmd;
            }
            prim += 0x68;
            i++;
        }
        for (i = 0; i < arg0->header->packet28_count; src++) {
            p = prim + (arg2 * 0x28);
            if (((*(u32 *)p & 0xFFFFFF) != 0) || (arg1 != 0)) {
                cmd = p[7];
                *(u32 *)(p + 4) = clutTbl[src->lookup_indices[0]];
                *(u32 *)(p + 0x10) = clutTbl[src->lookup_indices[1]];
                *(u32 *)(p + 0x1C) = clutTbl[src->lookup_indices[2]];
                p[7] = cmd;
            }
            prim += 0x50;
            i++;
        }
        for (i = 0; i < arg0->header->packet24_count; src++) {
            p = prim + (arg2 * 0x24);
            if (((*(u32 *)p & 0xFFFFFF) != 0) || (arg1 != 0)) {
                cmd = p[7];
                *(u32 *)(p + 4) = tpageTbl[src->lookup_indices[0]];
                *(u32 *)(p + 0xC) = tpageTbl[src->lookup_indices[1]];
                *(u32 *)(p + 0x14) = tpageTbl[src->lookup_indices[2]];
                *(u32 *)(p + 0x1C) = tpageTbl[src->lookup_indices[3]];
                p[7] = cmd;
            }
            prim += 0x48;
            i++;
        }
        for (i = 0; i < arg0->header->packet1c_count; src++) {
            p = prim + (arg2 * 0x1C);
            if (((*(u32 *)p & 0xFFFFFF) != 0) || (arg1 != 0)) {
                cmd = p[7];
                *(u32 *)(p + 4) = tpageTbl[src->lookup_indices[0]];
                *(u32 *)(p + 0xC) = tpageTbl[src->lookup_indices[1]];
                *(u32 *)(p + 0x14) = tpageTbl[src->lookup_indices[2]];
                p[7] = cmd;
            }
            prim += 0x38;
            i++;
        }
    }
}
