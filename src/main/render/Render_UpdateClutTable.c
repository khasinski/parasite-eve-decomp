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
            if ((((RenderPacket34 *)p)->tag & 0xFFFFFF) != 0 || (arg1 != 0)) {
                cmd = ((RenderPacket34 *)p)->values0.bytes.command;
                ((RenderPacket34 *)p)->values0.value = clutTbl[src->lookup_indices[0]];
                ((RenderPacket34 *)p)->value1 = clutTbl[src->lookup_indices[1]];
                ((RenderPacket34 *)p)->value2 = clutTbl[src->lookup_indices[2]];
                ((RenderPacket34 *)p)->value3 = clutTbl[src->lookup_indices[3]];
                ((RenderPacket34 *)p)->values0.bytes.command = cmd;
            }
            prim += 0x68;
            i++;
        }
        for (i = 0; i < arg0->header->packet28_count; src++) {
            p = prim + (arg2 * 0x28);
            if ((((RenderPacket28 *)p)->tag & 0xFFFFFF) != 0 || (arg1 != 0)) {
                cmd = ((RenderPacket28 *)p)->values0.bytes.command;
                ((RenderPacket28 *)p)->values0.value = clutTbl[src->lookup_indices[0]];
                ((RenderPacket28 *)p)->value1 = clutTbl[src->lookup_indices[1]];
                ((RenderPacket28 *)p)->value2 = clutTbl[src->lookup_indices[2]];
                ((RenderPacket28 *)p)->values0.bytes.command = cmd;
            }
            prim += 0x50;
            i++;
        }
        for (i = 0; i < arg0->header->packet24_count; src++) {
            p = prim + (arg2 * 0x24);
            if ((((RenderPacket24 *)p)->tag & 0xFFFFFF) != 0 || (arg1 != 0)) {
                cmd = ((RenderPacket24 *)p)->values0.bytes.command;
                ((RenderPacket24 *)p)->values0.value = tpageTbl[src->lookup_indices[0]];
                ((RenderPacket24 *)p)->value1 = tpageTbl[src->lookup_indices[1]];
                ((RenderPacket24 *)p)->value2 = tpageTbl[src->lookup_indices[2]];
                ((RenderPacket24 *)p)->value3 = tpageTbl[src->lookup_indices[3]];
                ((RenderPacket24 *)p)->values0.bytes.command = cmd;
            }
            prim += 0x48;
            i++;
        }
        for (i = 0; i < arg0->header->packet1c_count; src++) {
            p = prim + (arg2 * 0x1C);
            if ((((RenderPacket1C *)p)->tag & 0xFFFFFF) != 0 || (arg1 != 0)) {
                cmd = ((RenderPacket1C *)p)->values0.bytes.command;
                ((RenderPacket1C *)p)->values0.value = tpageTbl[src->lookup_indices[0]];
                ((RenderPacket1C *)p)->value1 = tpageTbl[src->lookup_indices[1]];
                ((RenderPacket1C *)p)->value2 = tpageTbl[src->lookup_indices[2]];
                ((RenderPacket1C *)p)->values0.bytes.command = cmd;
            }
            prim += 0x38;
            i++;
        }
    }
}
