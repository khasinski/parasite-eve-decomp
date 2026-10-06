/* CC1_FLAGS: -fno-strength-reduce */
/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/geom_state.h"
#include "pe1/render_prim.h"
#include "pe1/render_camera.h"

/* Matching debt: fixed-register pins and scheduling constraints preserve the stock PSYQ instruction schedule. */

#define LINK_PACKET(packet, ordering, mask24, maskTop)                     \
    do {                                                                   \
        u32 packet_tag = *(u32 *)(packet);             \
        u32 ot_tag = *(u32 *)(ordering);               \
        *(u32 *)(packet) = (packet_tag & (maskTop)) | (ot_tag & (mask24));  \
        *(u32 *)(ordering) = (ot_tag & maskTop) | (packet & mask24); \
    } while (0)

#define LINK_PACKET_PRELOADED(packet, ordering, packet_tag, mask24, maskTop) \
    do {                                                                    \
        u32 ot_tag;                                      \
        ot_tag = *(u32 *)(ordering);                                        \
        *(u32 *)(packet) = ((packet_tag) & (maskTop)) | (ot_tag & (mask24)); \
        ot_tag = *(u32 *)(ordering);                                        \
        *(u32 *)(ordering) = (ot_tag & (maskTop)) | ((u32)(packet) & (mask24)); \
    } while (0)

#define LINK_PAGE(packet, ordering, mask24, maskTop)                         \
    do {                                                                     \
        u32 packet_tag = *(u32 *)(packet);               \
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
    GeomEntry *entry;
    s32 active;
    RenderTilePacket *sprite;
    register RenderTexturePagePacket *page asm("$8");
    u8 *ordering;
    GeomState *state;
    u32 flags;
    u32 count;
    u32 i;
    register s32 scroll_x asm("$6");
    s32 scroll_y;
    s32 texture_base;
    register u32 *pos asm("$25");
    register u32 pos_offset asm("$3");
    s32 raw_x;
    register s32 raw_y asm("$7");
    register s32 screen_src asm("$3");
    char frame_pad[9];

    active = D_8009CDDC;
    asm("" : "=r"(active) : "0"(active));
    asm("" : "=r"(entry) : "0"(input));
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
        s32 screen_add;
        screen_src = entry->scr_x;
        screen_add = *(u16 *)((u8 *)state + 0x38);
        raw_x = screen_src + screen_add;
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
    pos = (u32 *)((u8 *)entry + pos_offset);

    if (flags & 4) {
        s32 mod_x;
        s32 mod_y;
        s32 divisor_x;
        s32 remainder_y;
        s32 signed_x;
        register s32 signed_y asm("$2");
        s32 base_y;
        signed_x = (s16)raw_x;
        mod_x = *(u16 *)((u8 *)entry + 4);
        signed_x -= 320;
        divisor_x = mod_x & 0xFFFF;
        signed_x += divisor_x;
        scroll_x = signed_x % divisor_x;
        mod_y = *(u16 *)((u8 *)entry + 6);
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
            u32 mask24 = 0x00FFFFFF;
            u32 maskTop = 0xFF000000;
            register RenderTexturePagePacket *page_cursor asm("$13") = page;
            register RenderTilePacket *sprite_cursor asm("$8") = sprite;
            register u32 *position_cursor asm("$10");
            position_cursor = pos;
                do {
                s32 draw_x;
                {
                    register u32 word_x asm("$2") = *position_cursor;
                    register s32 x asm("$4") = scroll_x + (word_x >> 22);
                    draw_x = x;
                    if ((s16)x >= 320) {
                        s32 modulus_x = entry->anim_mod_x;
                        draw_x = x - modulus_x;
                    } else if ((s16)x < -15) {
                        s32 modulus_x = entry->anim_mod_x;
                        draw_x = modulus_x + x;
                    }
                }
                if ((u16)(draw_x + 15) < 0x14F) {
                        register s32 draw_y asm("$5");
                        {
                            u32 word_y = *position_cursor;
                            s32 y = scroll_y + ((word_y >> 12) & 0x3FF);
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
                                u32 sprite_tag;
                                s32 ot_index = tile_u;
                                u8 *ot;
                                ot_index <<= 16;
                                ot_index >>= 14;
                                ot = (u8 *)(ot_index + (u32)ordering);
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
        u32 mask24;
        register u32 maskTop asm("$14");
        register RenderTilePacket *sprite_cursor asm("$5");
        RenderTexturePagePacket *page_cursor;
        register u32 *position_cursor asm("$10");
        i = 0;
        if (count != 0) {
            mask24 = 0x00FFFFFF;
            maskTop = 0xFF000000;
            sprite_cursor = sprite;
            page_cursor = page;
            position_cursor = pos;
            do {
            u32 word = *position_cursor;
            register s32 x asm("$2");
            register s32 draw_x asm("$13");
            x = scroll_x + (word >> 22);
            draw_x = x;
            if ((u16)(x + 15) < 0x14F) {
                    {
                    s32 y;
                    s32 draw_y;
                    s32 tile_u;
                    y = scroll_y + ((word >> 12) & 0x3FF);
                    draw_y = y;
                    if ((u16)(y + 15) < 0xEF) {
                        tile_u = texture_base + (*(u16 *)position_cursor & 0xFFF);
                        if ((u32)((tile_u - 8) & 0xFFFF) < 0xFF1) {
                            register u32 sprite_tag asm("$3");
                            s32 ot_index = tile_u;
                            u8 *ot;
                            ot_index <<= 16;
                            ot_index >>= 14;
                            ot = (u8 *)(ot_index + (u32)ordering);
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

extern int g_RenderStateFlags;

int Render_SetEntryVisible(int index, int enabled) {
    GeomStateAddress table, base;
    GeomEntry *entry = (GEOM_STATE_OFFSET(table, base, entry_offset, 0), table.entry) + index;

    if (enabled != 0) {
        entry->flags |= 2;
    } else {
        entry->flags &= 0xFD;
    }
    return 0;
}

int Render_SetEntryScrolled(int index, int enabled, unsigned int arg2, unsigned int arg3) {
    GeomStateAddress table, base;
    GeomEntry *entry = (GEOM_STATE_OFFSET(table, base, entry_offset, 0), table.entry) + index;

    if (enabled != 0) {
        entry->flags |= 4;
    } else {
        entry->flags &= 0xFB;
    }
    entry->field1C = arg2 >> 8;
    entry->field1E = arg3 >> 8;
    return 0;
}

int Render_SetEntryMirrored(int index, int enabled, unsigned int arg2, unsigned int arg3) {
    GeomStateAddress table, base;
    GeomEntry *entry = (GEOM_STATE_OFFSET(table, base, entry_offset, 0), table.entry) + index;

    if (enabled != 0) {
        entry->flags |= 8;
    } else {
        entry->flags &= 0xF7;
    }
    entry->field1C = (0x10000 - arg2) >> 8;
    entry->field1E = (0x10000 - arg3) >> 8;
    return 0;
}

int Render_SetEntryPosition(int index, int x, int y) {
    GeomStateAddress table, base;
    GeomEntry *entry = (GEOM_STATE_OFFSET(table, base, entry_offset, 0), table.entry) + index;
    volatile int *flags = &g_RenderStateFlags;

    entry->scr_x = x;
    entry->base_x = x;
    entry->scr_y = y;
    entry->base_y = y;
    *flags |= 0x80;
    return 0;
}

extern short D_800BD028, D_800BD02A;
extern u16 D_800BCFAC, D_800BCFAE, D_800BCFB0, D_800BCFB2;
extern u8 D_800BCFFA, D_800BCFFB;
int Gpu_LoadGeomState(int);
int Geo_RenderMeshList(void *buffer, void **end)
{
    s16 *minY;
    /* Reserve the otherwise unused slot in the retail 56-byte stack frame. */
    char frame_pad[2];
    u16 highY;
    u16 lowY;
    GeomState *state = D_800B1624;
    register u8 *group asm("$17") = &g_GeomGroupSel;
    CameraViewport *view;
    GeomEntry *entries;
    register unsigned int count asm("$19");
    unsigned int i;
    int x;
    Gpu_LoadGeomState(*group);
    view = (CameraViewport *)((u8 *)state + state->entry_offset_1C) + *group;
    x = (view->minX + view->maxX) / 2;
    {
        register short centerX = x;
        D_800BD028 = centerX;
        D_800BCF8C.x = centerX;
    }
    minY = &view->minY;
    D_800BCF8E = D_800BD02A = (*minY + view->maxY) / 2;
    asm volatile("":::"memory");
    D_800BCFAC = view->minX;
    asm volatile("":::"memory");
    D_800BCFAE = view->maxX;
    lowY = *minY;
    i = 0;
    D_800BCFB0 = lowY;
    asm volatile("":::"memory");
    highY = view->maxY;
    x -= 160;
    D_800BCFB2 = highY;
    asm volatile("":::"memory");
    count = state->entry_count06;
    state->out_disp_x = state->disp_src_x - x;
    asm("" ::: "memory");
    state->out_disp_y = state->disp_src_y - (D_800BCF8E - 112);
    entries = (GeomEntry *)((u8 *)state + state->entry_offset);
    *end = buffer;
    if (count) {
        int min = -32768, max = 32767;
        GeomEntry *entry = entries;
        do {
            if (Geo_LoadMeshEntry(entry, *end, end)) return -18;
            entry->ot10 = min;
            entry->ot12 = max;
            entry->ot14 = min;
            entry->ot16 = max;
            entry++;
        } while (++i < count);
    }
    D_800BCFFA = D_800BCFFB = 0;
    *(u32 *)&D_800BCF88.flags &= ~0xc00;
    return 0;
}
