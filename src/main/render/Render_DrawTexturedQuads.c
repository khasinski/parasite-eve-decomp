/* Matching debt: register pins, empty barriers, pointer/integer casts
 * and a 32-byte frame reservation. Individual GTE macros and hazard nops. */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_object.h"
#include "pe1/render_prim.h"
extern u32 D_800B1644[];
extern s32 D_800A636C[];
#define AT(base, off) ((u8 *)((u32)(off) + (u32)(base)))
void Render_DrawTexturedQuads(RenderObjectEntity *input) {
    u32 reserve[8];
    RenderObjectEntity *entity = input;
    int i = 0;
    u32 *xy;
    s32 *z;
    register RenderPrimitiveDescriptor *record asm("$15");
    register u8 *packets asm("$14");
    int slot;
    u32 *ordering;
    volatile s32 *area;

    /* Scheduling-only definition; area is assigned before any dereference. */
    asm("" : "=r"(area));
    asm("" : : "r"(area));
    xy = D_800B1644;
    z = D_800A636C;

    record = entity->primitive_descriptors;
    packets = entity->primitive_buffer;
    slot = D_8009CDDC;
    asm volatile("" : "=r"(slot) : "0"(slot) : "memory");
    {
        RenderObjectHeader *header = entity->header;
        int count;
        register int table_offset asm("$2");
        asm("" : "=r"(header) : "0"(header));
        table_offset = slot * 4;

        count = header->packet34_count;
        asm("" : "=r"(count) : "0"(count));
        ordering = *(u32 **)((u8 *)&D_800B0E38 + table_offset);
        asm volatile("" : "=r"(ordering) : "0"(ordering) : "$19");
        area = (volatile s32 *)0x1F800000;
        if (count > 0) {
            register int product asm("$2") = slot * 13;
            register int slot_offset asm("$22");
            register u32 address_mask asm("$24");
            register u32 length_mask asm("$20");
            register u16 *window asm("$10");
            asm("" : "=r"(product) : "0"(product));
            slot_offset = product * 4;

            address_mask = 0xFFFFFF;
            length_mask = 0xFF000000;
            window = &record->lookup_indices[3];

            do {
                register int a asm("$2") = window[-3];
                int b = window[-2];
                register int c asm("$4") = window[-1];
                int offset_a;
                int offset_c;
                u32 xy0;
                u32 xy1;
                u32 xy2;
                u8 *packet;
                u32 *coord;
                register int d asm("$5");
                asm("" : "=r"(a), "=r"(b), "=r"(c) : "0"(a), "1"(b), "2"(c));
                offset_a = a * 4;
                coord = (u32 *)AT(xy, offset_a);
                b *= 4;
                xy0 = *coord;
                coord = (u32 *)AT(xy, b);
                asm("" : "=r"(coord) : "0"(coord));
                offset_c = c * 4;
                xy1 = *coord;
                coord = (u32 *)AT(xy, offset_c);
                xy2 = *coord;
                gte_ldsxy0(xy0);
                gte_ldsxy2(xy2);
                gte_ldsxy1(xy1);
                gte_cop2_hazard_slot();
                gte_cop2_hazard_slot();
                gte_nclip_now();
                packet = packets + slot_offset;

                d = window[0];
                gte_stmac0(area);
                if (*area > 0) {
                    register s32 depth asm("$4");
                    {
                        register s32 *p0 asm("$2") = (s32 *)AT(z, offset_a);
                        s32 *p1 = (s32 *)AT(z, b);
                        register s32 value0 asm("$2");
                        s32 value1;
                        asm("" : "=r"(p0), "=r"(p1) : "0"(p0), "1"(p1));
                        depth = *p0;
                        value0 = *p1;
                        p1 = (s32 *)AT(z, offset_c);
                        d *= 4;
                        value1 = *p1;
                        depth += value0;
                        p0 = (s32 *)AT(z, d);
                        value0 = *p0;
                        depth += value1;
                        depth += value0;
                    }
                    depth >>= 4;
                    if (depth < 0x1000) {
                        u32 *entry;
                        depth *= 4;
                        entry = (u32 *)AT(ordering, depth);

                        ((RenderPacket34 *)packet)->tag =
                            (((RenderPacket34 *)packet)->tag & length_mask) |
                            (*entry & address_mask);
                        *entry = (*entry & length_mask) | ((u32)packet & address_mask);
                        asm volatile("" : "=r"(d) : "0"(d) : "memory");
                        {
                            register u32 *last asm("$2") = (u32 *)AT(xy, d);
                            asm("" : "=r"(last) : "0"(last));
                            ((RenderPacket34 *)packet)->sxy0 = xy0;
                            ((RenderPacket34 *)packet)->sxy1 = xy1;
                            ((RenderPacket34 *)packet)->sxy2 = xy2;
                            ((RenderPacket34 *)packet)->sxy3 = *last;
                        }
                    }
                } else {
                    ((RenderPacket34 *)packet)->tag &= length_mask;
                }
                packets += 104;
                i++;
                window += 6;
                record++;
            } while (i < entity->header->packet34_count);
        }
    }
    {
        int count = entity->header->packet28_count;

        i = 0;
        if (count > 0) {
            register int product asm("$2") = slot * 5;
            register int slot_offset asm("$20");
            u32 address_mask;
            register u32 length_mask asm("$24");
            register u16 *window asm("$8");
            asm("" : "=r"(product) : "0"(product));
            slot_offset = product * 8;

            address_mask = 0xFFFFFF;
            length_mask = 0xFF000000;
            window = &record->lookup_indices[2];
            asm(""
                : "=r"(slot_offset), "=r"(address_mask), "=r"(length_mask), "=r"(window)
                : "0"(slot_offset), "1"(address_mask), "2"(length_mask), "3"(window));
            do {
                register int a asm("$2") = window[-2];
                int b = window[-1];
                int c = window[0];
                int offset_a;
                int offset_c;
                u32 xy0;
                u32 xy1;
                register u32 xy2 asm("$9");
                u8 *packet;
                register u32 *coord asm("$2");
                asm("" : "=r"(a), "=r"(b), "=r"(c) : "0"(a), "1"(b), "2"(c));
                offset_a = a * 4;
                coord = (u32 *)AT(xy, offset_a);
                asm("" : "=r"(coord) : "0"(coord));
                b *= 4;
                xy0 = *coord;
                coord = (u32 *)AT(xy, b);
                asm("" : "=r"(coord) : "0"(coord));
                offset_c = c * 4;
                xy1 = *coord;
                coord = (u32 *)AT(xy, offset_c);
                asm("" : "=r"(coord) : "0"(coord));
                xy2 = *coord;
                gte_ldsxy0(xy0);
                gte_ldsxy2(xy2);
                gte_ldsxy1(xy1);
                gte_cop2_hazard_slot();
                gte_cop2_hazard_slot();
                gte_nclip_now();
                packet = packets + slot_offset;

                gte_stmac0(area);
                if (*area > 0) {
                    register s32 depth asm("$4");
                    {
                        s32 *p0 = (s32 *)AT(z, offset_a);
                        s32 *p1 = (s32 *)AT(z, b);
                        s32 *p2 = (s32 *)AT(z, offset_c);
                        register s32 value0 asm("$2");
                        s32 value1;
                        register s32 quotient asm("$2");
                        value0 = *p0;
                        value1 = *p1;
                        depth = *p2;
                        value0 += value1;
                        depth = value0 + depth;
                        quotient = depth / 3;

                        depth = quotient >> 2;
                    }
                    if (depth < 0x1000) {
                        u32 *entry;
                        depth *= 4;
                        entry = (u32 *)AT(ordering, depth);

                        ((RenderPacket28 *)packet)->tag =
                            (((RenderPacket28 *)packet)->tag & length_mask) |
                            (*entry & address_mask);
                        *entry = (*entry & length_mask) | ((u32)packet & address_mask);
                        ((RenderPacket28 *)packet)->sxy0 = xy0;
                        ((RenderPacket28 *)packet)->sxy1 = xy1;
                        ((RenderPacket28 *)packet)->sxy2 = xy2;
                    }
                } else {
                    ((RenderPacket28 *)packet)->tag &= length_mask;
                }
                packets += 80;
                i++;
                window += 6;
                record++;
            } while (i < entity->header->packet28_count);
        }
    }
    {
        int count = entity->header->packet24_count;

        i = 0;
        if (count > 0) {
            register int product asm("$2") = slot * 9;
            register int slot_offset asm("$22");
            register u32 address_mask asm("$24");
            register u32 length_mask asm("$20");
            register u16 *window asm("$10");
            asm("" : "=r"(product) : "0"(product));
            slot_offset = product * 4;

            address_mask = 0xFFFFFF;
            length_mask = 0xFF000000;
            window = &record->lookup_indices[3];

            do {
                register int a asm("$2") = window[-3];
                int b = window[-2];
                register int c asm("$4") = window[-1];
                int offset_a;
                int offset_c;
                u32 xy0;
                u32 xy1;
                u32 xy2;
                u8 *packet;
                u32 *coord;
                register int d asm("$5");
                asm("" : "=r"(a), "=r"(b), "=r"(c) : "0"(a), "1"(b), "2"(c));
                offset_a = a * 4;
                coord = (u32 *)AT(xy, offset_a);
                b *= 4;
                xy0 = *coord;
                coord = (u32 *)AT(xy, b);
                asm("" : "=r"(coord) : "0"(coord));
                offset_c = c * 4;
                xy1 = *coord;
                coord = (u32 *)AT(xy, offset_c);
                xy2 = *coord;
                gte_ldsxy0(xy0);
                gte_ldsxy2(xy2);
                gte_ldsxy1(xy1);
                gte_cop2_hazard_slot();
                gte_cop2_hazard_slot();
                gte_nclip_now();
                packet = packets + slot_offset;

                d = window[0];
                gte_stmac0(area);
                if (*area > 0) {
                    register s32 depth asm("$4");
                    {
                        register s32 *p0 asm("$2") = (s32 *)AT(z, offset_a);
                        s32 *p1 = (s32 *)AT(z, b);
                        register s32 value0 asm("$2");
                        s32 value1;
                        asm("" : "=r"(p0), "=r"(p1) : "0"(p0), "1"(p1));
                        depth = *p0;
                        value0 = *p1;
                        p1 = (s32 *)AT(z, offset_c);
                        d *= 4;
                        value1 = *p1;
                        depth += value0;
                        p0 = (s32 *)AT(z, d);
                        value0 = *p0;
                        depth += value1;
                        depth += value0;
                    }
                    depth >>= 4;
                    if (depth < 0x1000) {
                        u32 *entry;
                        depth *= 4;
                        entry = (u32 *)AT(ordering, depth);

                        ((RenderPacket24 *)packet)->tag =
                            (((RenderPacket24 *)packet)->tag & length_mask) |
                            (*entry & address_mask);
                        *entry = (*entry & length_mask) | ((u32)packet & address_mask);
                        asm volatile("" : "=r"(d) : "0"(d) : "memory");
                        {
                            register u32 *last asm("$2") = (u32 *)AT(xy, d);
                            asm("" : "=r"(last) : "0"(last));
                            ((RenderPacket24 *)packet)->sxy0 = xy0;
                            ((RenderPacket24 *)packet)->sxy1 = xy1;
                            ((RenderPacket24 *)packet)->sxy2 = xy2;
                            ((RenderPacket24 *)packet)->sxy3 = *last;
                        }
                    }
                } else {
                    ((RenderPacket24 *)packet)->tag &= length_mask;
                }
                packets += 72;
                i++;
                window += 6;
                record++;
            } while (i < entity->header->packet24_count);
        }
    }
    {
        int count = entity->header->packet1c_count;

        i = 0;
        if (count > 0) {
            register int product asm("$2") = slot * 7;
            register int slot_offset asm("$20");
            u32 address_mask;
            register u32 length_mask asm("$18");
            register u16 *window asm("$8");
            asm("" : "=r"(product) : "0"(product));
            slot_offset = product * 4;

            address_mask = 0xFFFFFF;
            length_mask = 0xFF000000;
            window = &record->lookup_indices[2];

            do {
                register int a asm("$2") = window[-2];
                int b = window[-1];
                int c = window[0];
                int offset_a;
                int offset_c;
                register u32 xy0 asm("$12");
                register u32 xy1 asm("$11");
                register u32 xy2 asm("$9");
                u8 *packet;
                register u32 *coord asm("$2");
                asm("" : "=r"(a), "=r"(b), "=r"(c) : "0"(a), "1"(b), "2"(c));
                offset_a = a * 4;
                coord = (u32 *)AT(xy, offset_a);
                asm("" : "=r"(coord) : "0"(coord));
                b *= 4;
                xy0 = *coord;
                coord = (u32 *)AT(xy, b);
                asm("" : "=r"(coord) : "0"(coord));
                offset_c = c * 4;
                xy1 = *coord;
                coord = (u32 *)AT(xy, offset_c);
                asm("" : "=r"(coord) : "0"(coord));
                xy2 = *coord;
                gte_ldsxy0(xy0);
                gte_ldsxy2(xy2);
                gte_ldsxy1(xy1);
                gte_cop2_hazard_slot();
                gte_cop2_hazard_slot();
                gte_nclip_now();
                packet = packets + slot_offset;

                gte_stmac0(area);
                if (*area > 0) {
                    register s32 depth asm("$4");
                    {
                        s32 *p0 = (s32 *)AT(z, offset_a);
                        s32 *p1 = (s32 *)AT(z, b);
                        s32 *p2 = (s32 *)AT(z, offset_c);
                        s32 value0;
                        s32 value1;
                        register s32 quotient asm("$2");
                        value0 = *p0;
                        value1 = *p1;
                        depth = *p2;
                        value0 += value1;
                        depth = value0 + depth;
                        quotient = depth / 3;

                        depth = quotient >> 2;
                    }
                    if (depth < 0x1000) {
                        u32 *entry;
                        depth *= 4;
                        entry = (u32 *)AT(ordering, depth);

                        ((RenderPacket1C *)packet)->tag =
                            (((RenderPacket1C *)packet)->tag & length_mask) |
                            (*entry & address_mask);
                        *entry = (*entry & length_mask) | ((u32)packet & address_mask);
                        ((RenderPacket1C *)packet)->sxy0 = xy0;
                        ((RenderPacket1C *)packet)->sxy1 = xy1;
                        ((RenderPacket1C *)packet)->sxy2 = xy2;
                    }
                } else {
                    ((RenderPacket1C *)packet)->tag &= length_mask;
                }
                packets += 56;
                i++;
                window += 6;
                record++;
            } while (i < entity->header->packet1c_count);
        }
    }
}
