#include "include_asm.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed short s16;

extern int D_8009CDDC;
extern u32 *D_800B0E38[];
extern void *D_800B1624;

int Render_DrawSpriteEntry(void *entry_arg) {
    u8 *entry = entry_arg;
    u8 *geom = D_800B1624;
    u32 *ot_base;
    u32 *ot_entry;
    u32 *prim_a;
    u32 *prim_b;
    u32 *packets;
    u32 packet;
    u32 mask;
    u32 tag_mask;
    int draw_slot;
    int count;
    int i;
    int x_base;
    int y_base;
    int screen_x;
    int screen_y;
    int width;
    int height;
    int z_base;
    int z;
    int u;
    int v;
    int wrapped;

    draw_slot = D_8009CDDC;
    count = *(u16 *)(entry + 0x26);
    prim_a = *(u32 **)(entry + 0x30);
    ot_base = D_800B0E38[draw_slot];
    if (draw_slot != 0) {
        prim_a = (u32 *)((u8 *)prim_a + count * 0x10);
    }

    prim_b = *(u32 **)(entry + 0x34);
    if (draw_slot != 0) {
        prim_b = (u32 *)((u8 *)prim_b + count * 8);
    }

    screen_x = *(u16 *)(entry + 0x0C) + *(u16 *)(geom + 0x38);
    x_base = screen_x;
    screen_y = *(u16 *)(entry + 0x0E) + *(u16 *)(geom + 0x3A);
    y_base = screen_y;
    z_base = *(u16 *)(geom + 0x26) + ((*(u32 *)entry >> 8) & 0xFFF);
    packets = (u32 *)(entry + *(u32 *)(entry + 0x28));

    if ((entry[0] & 4) != 0) {
        width = *(u16 *)(entry + 4);
        x_base = ((s16)screen_x - 0x140 + width) % width;
        x_base -= width - 0x140;

        height = *(u16 *)(entry + 6);
        y_base = ((s16)screen_y - 0xE0 + height) % height;
        y_base -= height - 0xE0;

        if (count != 0) {
            mask = 0x00FFFFFF;
            tag_mask = 0xFF000000;
            for (i = 0; i < count; i++) {
                packet = packets[i];
                u = x_base + (packet >> 22);
                wrapped = (s16)u;
                if (wrapped >= 0x140) {
                    u -= *(u16 *)(entry + 4);
                } else if (wrapped < -0xF) {
                    u += *(u16 *)(entry + 4);
                }

                if ((u16)(u + 0xF) < 0x14F) {
                    v = y_base + ((packet >> 12) & 0x3FF);
                    wrapped = (s16)v;
                    if (wrapped >= 0xE0) {
                        v -= *(u16 *)(entry + 6);
                    } else if (wrapped < -0xF) {
                        v += *(u16 *)(entry + 6);
                    }

                    if ((u16)(v + 0xF) < 0xEF) {
                        z = z_base + (*(u16 *)&packets[i] & 0xFFF);
                        if ((u16)(z - 8) < 0xFF1) {
                            ot_entry = ot_base + ((z << 16) >> 14);
                            *(u16 *)((u8 *)prim_a + 8) = u;
                            *(u16 *)((u8 *)prim_a + 0xA) = v;

                            prim_a[0] = (prim_a[0] & tag_mask) | (*ot_entry & mask);
                            *ot_entry = (*ot_entry & tag_mask) | ((u32)prim_a & mask);
                            prim_b[0] = (prim_b[0] & tag_mask) | (*ot_entry & mask);
                            *ot_entry = (*ot_entry & tag_mask) | ((u32)prim_b & mask);
                        }
                    }
                }

                prim_b = (u32 *)((u8 *)prim_b + 8);
                prim_a = (u32 *)((u8 *)prim_a + 0x10);
            }
        }
    } else if (count != 0) {
        mask = 0x00FFFFFF;
        tag_mask = 0xFF000000;
        for (i = 0; i < count; i++) {
            packet = packets[i];
            u = x_base + (packet >> 22);
            if ((u16)(u + 0xF) < 0x14F) {
                v = y_base + ((packet >> 12) & 0x3FF);
                if ((u16)(v + 0xF) < 0xEF) {
                    z = z_base + (*(u16 *)&packets[i] & 0xFFF);
                    if ((u16)(z - 8) < 0xFF1) {
                        ot_entry = ot_base + ((z << 16) >> 14);
                        *(u16 *)((u8 *)prim_a + 8) = u;
                        *(u16 *)((u8 *)prim_a + 0xA) = v;

                        prim_a[0] = (prim_a[0] & tag_mask) | (*ot_entry & mask);
                        *ot_entry = (*ot_entry & tag_mask) | ((u32)prim_a & mask);
                        prim_b[0] = (prim_b[0] & tag_mask) | (*ot_entry & mask);
                        *ot_entry = (*ot_entry & tag_mask) | ((u32)prim_b & mask);
                    }
                }
            }

            prim_b = (u32 *)((u8 *)prim_b + 8);
            prim_a = (u32 *)((u8 *)prim_a + 0x10);
        }
    }

    *(u16 *)(entry + 0x18) = x_base;
    *(u16 *)(entry + 0x1A) = y_base;
    return 0;
}
