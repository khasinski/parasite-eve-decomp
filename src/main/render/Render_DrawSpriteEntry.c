/* Matching debt: fixed-register pins and scheduling constraints preserve the stock PSYQ instruction schedule. */
#include "common.h"
/* MASPSX_FLAGS: --expand-div */
#include "pe1/geom_state.h"
#include "pe1/render_prim.h"

#define LINK_PACKET(packet, ordering, mask24, maskTop)                     \
    do {                                                                   \
        register u32 packet_tag asm("$3") = *(u32 *)(packet);             \
        register u32 ot_tag asm("$2") = *(u32 *)(ordering);               \
        *(u32 *)(packet) = (packet_tag & (maskTop)) | (ot_tag & (mask24));  \
        *(u32 *)(ordering) = (ot_tag & (maskTop)) | ((u32)(packet) & (mask24)); \
    } while (0)

#define LINK_PACKET_PRELOADED(packet, ordering, packet_tag, mask24, maskTop) \
    do {                                                                    \
        register u32 ot_tag asm("$2");                                      \
        ot_tag = *(u32 *)(ordering);                                        \
        *(u32 *)(packet) = ((packet_tag) & (maskTop)) | (ot_tag & (mask24)); \
        ot_tag = *(u32 *)(ordering);                                        \
        *(u32 *)(ordering) = (ot_tag & (maskTop)) | ((u32)(packet) & (mask24)); \
    } while (0)

#define LINK_PAGE(packet, ordering, mask24, maskTop)                         \
    do {                                                                     \
        register u32 packet_tag asm("$3") = *(u32 *)(packet);               \
        register u32 ot_tag asm("$2");                                      \
        ot_tag = *(u32 *)(ordering);                                         \
        ot_tag &= (mask24);                                                   \
        packet_tag &= (maskTop);                                              \
        packet_tag |= ot_tag;                                                 \
        *(u32 *)(packet) = packet_tag;                                        \
        ot_tag = *(u32 *)(ordering);                                         \
        *(u32 *)(ordering) = (ot_tag & (maskTop)) | ((u32)(packet) & (mask24)); \
    } while (0)

int Render_DrawSpriteEntry(GeomEntry *input)
{
    register GeomEntry *entry asm("$11");
    register s32 active asm("$3");
    register RenderTilePacket *sprite asm("$10");
    register RenderTexturePagePacket *page asm("$8");
    register u8 *ordering asm("$17");
    register GeomState *state asm("$4");
    register u32 flags asm("$2");
    register u32 count asm("$15");
    register u32 i asm("$12");
    register s32 scroll_x asm("$6");
    register s32 scroll_y asm("$24");
    register s32 texture_base asm("$16");
    register u32 *pos asm("$25");
    register u32 pos_offset asm("$3");
    register s32 raw_x asm("$5");
    register s32 raw_y asm("$7");
    register s32 screen_src asm("$3");
    volatile char frame_pad[9];

    active = D_8009CDDC;
    asm("" : "=r"(active) : "0"(active));
    asm("" : "=r"(entry) : "0"(input));
    asm volatile("" : : "m"(frame_pad[0]), "m"(frame_pad[1]));
    sprite = (RenderTilePacket *)entry->u30.prim;
    asm("" : "=r"(sprite) : "0"(sprite));
    count = entry->prim_count;
    asm("" : "=r"(count) : "0"(count));
    ordering = (u8 *)D_800B0E38.ordering[active];
    if (active != 0) {
        sprite += count;
    }
    page = entry->u34.pagePackets;
    if (active != 0)
        page += count;

    state = D_800B1624;
    {
        register s32 screen_add asm("$2");
        screen_src = entry->scr_x;
        screen_add = *(u16 *)((u8 *)state + 0x38);
        raw_x = screen_src + screen_add;
        asm volatile("" : : "r"(raw_x) : "memory");
        screen_src = entry->scr_y;
        screen_add = *(u16 *)((u8 *)state + 0x3A);
        scroll_x = raw_x;
        raw_y = screen_src + screen_add;
        scroll_y = raw_y;
    }
    asm volatile("" : : "r"(scroll_x), "r"(scroll_y) : "memory");
    {
        register u32 texture_word asm("$2");
        texture_word = *(u32 *)entry;
        texture_word >>= 8;
        texture_word &= 0xFFF;
        screen_src = state->field26;
        texture_base = screen_src + texture_word;
    }
    pos_offset = entry->u28.pos_ptr;
    flags = entry->flags;
    asm volatile("" : : "r"(flags) : "memory");
    pos = (u32 *)((u8 *)entry + pos_offset);

    if (flags & 4) {
        register s32 mod_x asm("$5");
        register s32 mod_y asm("$4");
        register s32 divisor_x asm("$3");
        register s32 remainder_y asm("$3");
        register s32 signed_x asm("$2");
        register s32 signed_y asm("$2");
        register s32 base_y asm("$4");
        signed_x = (s16)raw_x;
        asm("" : "=r"(signed_x) : "0"(signed_x));
        mod_x = *(volatile u16 *)((u8 *)entry + 4);
        signed_x -= 320;
        asm volatile("" : "=r"(mod_x) : "0"(mod_x));
        divisor_x = mod_x & 0xFFFF;
        signed_x += divisor_x;
        scroll_x = signed_x % divisor_x;
        mod_y = *(volatile u16 *)((u8 *)entry + 6);
        asm volatile("" : : "r"(mod_y) : "memory");
        signed_y = (s16)raw_y;
        signed_y -= 224;
        signed_y += mod_y & 0xFFFF;
        remainder_y = signed_y % (mod_y & 0xFFFF);
        i = 0;
        raw_x = mod_x - 320;
        scroll_x -= raw_x;
        base_y = mod_y;
        base_y -= 224;
        scroll_y = remainder_y - base_y;
        if (count != 0) {
            register u32 mask24 asm("$9") = 0x00FFFFFF;
            register u32 maskTop asm("$14") = 0xFF000000;
            register RenderTexturePagePacket *page_cursor asm("$13") = page;
            register RenderTilePacket *sprite_cursor asm("$8") = sprite;
            register u32 *position_cursor asm("$10");
            position_cursor = pos;
                do {
                register s32 draw_x asm("$7");
                {
                    register u32 word_x asm("$2") = *position_cursor;
                    register s32 x asm("$4") = scroll_x + (word_x >> 22);
                    draw_x = x;
                    if ((s16)x >= 320) {
                        register s32 modulus_x asm("$2") = entry->anim_mod_x;
                        draw_x = x - modulus_x;
                    } else if ((s16)x < -15) {
                        register s32 modulus_x asm("$2") = entry->anim_mod_x;
                        draw_x = modulus_x + x;
                    }
                }
                if ((u16)(draw_x + 15) < 0x14F) {
                        register s32 draw_y asm("$5");
                        {
                            register u32 word_y asm("$2") = *position_cursor;
                            register s32 y asm("$4") = scroll_y + ((word_y >> 12) & 0x3FF);
                            draw_y = y;
                            if ((s16)y >= 224) {
                                register s32 modulus_y asm("$2") = entry->anim_mod_y;
                                draw_y = y - modulus_y;
                            } else if ((s16)y < -15) {
                                register s32 modulus_y asm("$2") = entry->anim_mod_y;
                                draw_y = modulus_y + y;
                            }
                        }
                        if ((u16)(draw_y + 15) < 0xEF) {
                            s32 tile_u = texture_base + (*(u16 *)position_cursor & 0xFFF);
                            if ((u32)((tile_u - 8) & 0xFFFF) < 0xFF1) {
                                register u32 sprite_tag asm("$3");
                                register s32 ot_index asm("$4") = tile_u;
                                u8 *ot;
                                ot_index <<= 16;
                                ot_index >>= 14;
                                ot = (u8 *)((u32)ot_index + (u32)ordering);
                                sprite_tag = *(u32 *)sprite_cursor;
                                ((RenderTilePacket *)sprite_cursor)->x = draw_x;
                                ((RenderTilePacket *)sprite_cursor)->y = draw_y;
                                LINK_PACKET_PRELOADED(sprite_cursor, ot, sprite_tag, mask24, maskTop);
                                LINK_PAGE(page_cursor, ot, mask24, maskTop);
                            }
                        }
                }
                page_cursor += 1;
                sprite_cursor += 1;
                ++i;
                ++position_cursor;
            } while (i < count);
        }
    } else {
        register u32 mask24 asm("$7");
        register u32 maskTop asm("$14");
        register RenderTilePacket *sprite_cursor asm("$5");
        register RenderTexturePagePacket *page_cursor asm("$8");
        register u32 *position_cursor asm("$10");
        i = 0;
        if (count != 0) {
            mask24 = 0x00FFFFFF;
            maskTop = 0xFF000000;
            sprite_cursor = sprite;
            page_cursor = page;
            position_cursor = pos;
            do {
            register u32 word asm("$3") = *position_cursor;
            register s32 x asm("$2");
            register s32 draw_x asm("$13");
            x = scroll_x + (word >> 22);
            asm("" : "=r"(x) : "0"(x));
            draw_x = x;
            if ((u16)(x + 15) < 0x14F) {
                    {
                    register s32 y asm("$2");
                    register s32 draw_y asm("$9");
                    s32 tile_u;
                    y = scroll_y + ((word >> 12) & 0x3FF);
                    asm("" : "=r"(y) : "0"(y));
                    draw_y = y;
                    if ((u16)(y + 15) < 0xEF) {
                        tile_u = texture_base + (*(u16 *)position_cursor & 0xFFF);
                        if ((u32)((tile_u - 8) & 0xFFFF) < 0xFF1) {
                            register u32 sprite_tag asm("$3");
                            register s32 ot_index asm("$4") = tile_u;
                            u8 *ot;
                            ot_index <<= 16;
                            ot_index >>= 14;
                            ot = (u8 *)((u32)ot_index + (u32)ordering);
                            sprite_tag = *(u32 *)sprite_cursor;
                            ((RenderTilePacket *)sprite_cursor)->x = draw_x;
                            ((RenderTilePacket *)sprite_cursor)->y = draw_y;
                            LINK_PACKET_PRELOADED(sprite_cursor, ot, sprite_tag, mask24, maskTop);
                            LINK_PAGE(page_cursor, ot, mask24, maskTop);
                        }
                    }
                    }
                }
            page_cursor += 1;
            sprite_cursor += 1;
            ++i;
            ++position_cursor;
            } while (i < count);
        }
    }

    entry->disp_x = scroll_x;
    entry->disp_y = scroll_y;
    return 0;
}
