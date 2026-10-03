#include "pe1/gte.h"
#include "pe1/render_object.h"
#include "pe1/render_prim.h"
#include "pe1/render_tint.h"
#ifndef ROOMFX_MODEL_DRAW_FUNC
#error "ROOMFX_MODEL_DRAW_FUNC must name the room entry point"
#endif

/* Projected screen XY and view depth of the transformed model vertices. */
extern u32 D_800B1644[];
extern s32 D_800A636C[];

/* PSY-Q libgpu style ordering-table link through the 24-bit tag address. */
#define ROOMFX_SETADDR(p, a) (((RenderGpuTag *)(p))->address = (u32)(a))
#define ROOMFX_GETADDR(p) (((RenderGpuTag *)(p))->address)
#define ROOMFX_ADDPRIM(ot, p) \
    ROOMFX_SETADDR(p, ROOMFX_GETADDR(ot)), ROOMFX_SETADDR(ot, p)

/* Room build of the main binary's Render_DrawTexturedQuads: NCLIP-tests each
 * face on the projected vertices and links the front-facing ones (negative
 * winding) into the active ordering table at their average depth; culled
 * faces get a null link. Quads and triangles of both packet classes are
 * walked in turn through one shared descriptor cursor. */
void ROOMFX_MODEL_DRAW_FUNC(RenderObjectEntity *entity) {
    int i;
    s32 *area = (s32 *)0x1F800000;
    u32 *xy = D_800B1644;
    RenderPrimitiveDescriptor *record = entity->primitive_descriptors;
    u8 *packets = entity->primitive_buffer;
    int slot = D_8009CDDC;
    u32 *ordering = (u32 *)D_800B0E38.ordering[slot];
    s32 *z = D_800A636C;
    u32 xy0, xy1, xy2;
    int a, b, c, d;
    u8 *packet;

    i = 0;
    while (i < entity->header->packet34_count) {
        a = record->lookup_indices[0];
        b = record->lookup_indices[1];
        c = record->lookup_indices[2];
        xy0 = xy[a];
        xy1 = xy[b];
        xy2 = xy[c];
        gte_ldsxy0(xy0);
        gte_ldsxy2(xy2);
        gte_ldsxy1(xy1);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_nclip_now();
        packet = (u8 *)((RenderPacket34 *)packets + slot);
        d = record->lookup_indices[3];
        gte_stmac0(area);
        if (*area < 0) {
            int depth = z[a] + z[b];
            depth += z[c];
            depth += z[d];
            depth >>= 4;
            if (depth < 0x1000) {
                u32 *entry = (u32 *)(depth * 4 + (u32)ordering);
                ROOMFX_ADDPRIM(entry, packet);
                ((RenderPacket34 *)packet)->sxy0 = xy0;
                ((RenderPacket34 *)packet)->sxy1 = xy1;
                ((RenderPacket34 *)packet)->sxy2 = xy2;
                ((RenderPacket34 *)packet)->sxy3 = xy[d];
            }
        } else {
            ROOMFX_SETADDR(packet, 0);
        }
        packets += 2 * sizeof(RenderPacket34);
        i++;
        record++;
    }

    i = 0;
    while (i < entity->header->packet28_count) {
        a = record->lookup_indices[0];
        b = record->lookup_indices[1];
        c = record->lookup_indices[2];
        xy0 = xy[a];
        xy1 = xy[b];
        xy2 = xy[c];
        gte_ldsxy0(xy0);
        gte_ldsxy2(xy2);
        gte_ldsxy1(xy1);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_nclip_now();
        packet = (u8 *)((RenderPacket28 *)packets + slot);
        gte_stmac0(area);
        if (*area < 0) {
            int depth = z[a] + z[b] + z[c];
            depth = depth / 3 >> 2;
            if (depth < 0x1000) {
                u32 *entry = (u32 *)(depth * 4 + (u32)ordering);
                ROOMFX_ADDPRIM(entry, packet);
                ((RenderPacket28 *)packet)->sxy0 = xy0;
                ((RenderPacket28 *)packet)->sxy1 = xy1;
                ((RenderPacket28 *)packet)->sxy2 = xy2;
            }
        } else {
            ROOMFX_SETADDR(packet, 0);
        }
        packets += 2 * sizeof(RenderPacket28);
        i++;
        record++;
    }

    i = 0;
    while (i < entity->header->packet24_count) {
        a = record->lookup_indices[0];
        b = record->lookup_indices[1];
        c = record->lookup_indices[2];
        xy0 = xy[a];
        xy1 = xy[b];
        xy2 = xy[c];
        gte_ldsxy0(xy0);
        gte_ldsxy2(xy2);
        gte_ldsxy1(xy1);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_nclip_now();
        packet = (u8 *)((RenderPacket24 *)packets + slot);
        d = record->lookup_indices[3];
        gte_stmac0(area);
        if (*area < 0) {
            int depth = z[a] + z[b];
            depth += z[c];
            depth += z[d];
            depth >>= 4;
            if (depth < 0x1000) {
                u32 *entry = (u32 *)(depth * 4 + (u32)ordering);
                ROOMFX_ADDPRIM(entry, packet);
                ((RenderPacket24 *)packet)->sxy0 = xy0;
                ((RenderPacket24 *)packet)->sxy1 = xy1;
                ((RenderPacket24 *)packet)->sxy2 = xy2;
                ((RenderPacket24 *)packet)->sxy3 = xy[d];
            }
        } else {
            ROOMFX_SETADDR(packet, 0);
        }
        packets += 2 * sizeof(RenderPacket24);
        i++;
        record++;
    }

    i = 0;
    while (i < entity->header->packet1c_count) {
        a = record->lookup_indices[0];
        b = record->lookup_indices[1];
        c = record->lookup_indices[2];
        xy0 = xy[a];
        xy1 = xy[b];
        xy2 = xy[c];
        gte_ldsxy0(xy0);
        gte_ldsxy2(xy2);
        gte_ldsxy1(xy1);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_nclip_now();
        packet = (u8 *)((RenderPacket1C *)packets + slot);
        gte_stmac0(area);
        if (*area < 0) {
            int depth = z[a] + z[b] + z[c];
            depth = depth / 3 >> 2;
            if (depth < 0x1000) {
                u32 *entry = (u32 *)(depth * 4 + (u32)ordering);
                ROOMFX_ADDPRIM(entry, packet);
                ((RenderPacket1C *)packet)->sxy0 = xy0;
                ((RenderPacket1C *)packet)->sxy1 = xy1;
                ((RenderPacket1C *)packet)->sxy2 = xy2;
            }
        } else {
            ROOMFX_SETADDR(packet, 0);
        }
        packets += 2 * sizeof(RenderPacket1C);
        i++;
        record++;
    }
}

#undef ROOMFX_ADDPRIM
#undef ROOMFX_GETADDR
#undef ROOMFX_SETADDR
#undef ROOMFX_MODEL_DRAW_FUNC
