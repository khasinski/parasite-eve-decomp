/* CC1_FLAGS: -fno-strength-reduce */
/* MASPSX_FLAGS: --expand-div */
/* Geometry sprite entries and mesh lists, the screen fade and tint, the
 * background scroll/parallax step and the room shadow and primitive colour
 * draws (G0833/G0835). */
#include "common.h"
#include "pe1/geom_state.h"
#include "pe1/render_prim.h"
#include "pe1/render_camera.h"
#include "pe1/psyq_nop.h"
#include "pe1/game_state.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/field_tile.h"
#include "pe1/field_anim.h"
#include "pe1/render_shadow.h"

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
    entry = input;
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
        RenderTilePacket *sprite_cursor;
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
    *(u32 *)&D_800BCF88.state.flags &= ~0xc00;
    return 0;
}

/* Field geometry: clamp the display-source point into the clip window and
 * draw the selected group's sprite entries relative to the camera.
 * Contiguous default-profile pair on g_GeomState (main_tu_evidence G0836
 * joins them with the mesh renderer before and the fades after). */

/* Camera y read through an array view: the scalar form schedules the load
 * before the preceding store. */
extern u16 g_CameraClampedY[];


int Geo_ClipPoint(int x, int y, int z) {
    GeomState *state = g_GeomState;
    register int coordinate asm("$3");
    register int bound asm("$2");
    int clippedX;
    int clippedY;
    register int depth asm("$4");
    int savedBound;
    /* Retail reserves 32 bytes even though the function makes no calls.
     * Pins and identity barriers preserve its separate comparison/copy values. */
    int matchingStackReserve[8];

    coordinate = state->clip_offset_x + x;
    asm("" : "=r"(clippedX) : "0"(coordinate));
    bound = state->clip_offset_y;
    clippedY = bound + y;
    coordinate = (s16)coordinate;
    depth = state->depth_offset;

    bound = state->clip_min_x;
    /* Preserve the original empty load-delay slot. */
    PE1_NOP();
    asm("" : "=r"(savedBound) : "0"(bound));
    depth = depth + z;
    if (coordinate < bound) {
        clippedX = savedBound;
    } else {
        bound = state->clip_max_x;
        asm("" : "=r"(savedBound) : "0"(bound));
        if (bound < coordinate) {
            clippedX = savedBound;
        }
    }

    coordinate = clippedY << 16;
    bound = state->clip_min_y;
    coordinate = coordinate >> 16;
    asm("" : "=r"(savedBound) : "0"(bound));
    if (coordinate < bound) {
        clippedY = savedBound;
    } else {
        bound = state->clip_max_y;
        asm("" : "=r"(savedBound) : "0"(bound));
        if (bound < coordinate) {
            clippedY = savedBound;
        }
    }

    state->disp_src_x = clippedX;
    state->disp_src_y = clippedY;
    state->field26 = depth;
    return 0;
}

s32 Geo_BuildMeshList(void) {
    register GeomStateAddress base asm("$4");
    GeomEntry *entry;
    s32 i;
    register u16 count asm("$18");
    s32 temp;
    register s32 value asm("$3");
    s32 stack_pad[2];

    base.state = g_GeomState;
    temp = (u16)D_800BCF8C.x;
    temp -= 0xA0;
    value = base.state->disp_src_x;
    count = base.state->entry_count06;
    value -= temp;
    base.state->out_disp_x = value;
    temp = g_CameraClampedY[0];
    value = base.state->disp_src_y;
    temp -= 0x70;
    value -= temp;
    temp = base.state->entry_offset;
    i = 0;
    base.state->out_disp_y = value;
    base.word += temp;
    if (count != 0) {
        entry = base.entry;
        do {
            if ((entry->flags & 2) && (entry->group == g_GeomGroupSel)) {
                Render_DrawSpriteEntry(entry);
            }
            i += 1;
            entry++;
        } while (i < count);
    }
    return 0;
}

extern u8 g_RenderFadeFrameCount;
extern u8 g_RenderFadeFrameCounter;
int Render_StartFadeIn(int arg0)
{
  int new_var;
  register int *flags;
  int value;
  flags = &g_RenderStateFlags;
  g_RenderFadeFrameCount = arg0;
  g_RenderFadeFrameCounter = 0;
  new_var = *flags;
  value = new_var;
  value &= ~0xC00;
  value |= 0x400;
  *flags = value;
  return 0;
}

extern struct { char _[16]; } D_800BCF88_o __asm__("D_800BCF88");
extern struct { char _[16]; } D_800BCF88_store_o __asm__("D_800BCF88");
extern struct { char _[16]; } D_800BCFFA_o __asm__("D_800BCFFA");
extern struct { char _[16]; } D_800BCFFB_o __asm__("D_800BCFFB");
extern struct { char _[16]; } D_800B1624_a_o __asm__("D_800B1624");
extern struct { char _[16]; } D_800B1624_b_o __asm__("D_800B1624");
extern struct { char _[16]; } D_800B1624_c_o __asm__("D_800B1624");

#define D_800BCF88_WORD (*(s32 *)&D_800BCF88_o)
#define D_800BCFFA_BYTE (*(u8 *)&D_800BCFFA_o)
#define D_800BCFFB_BYTE (*(u8 *)&D_800BCFFB_o)
#define D_800BCFFB_PTR ((u8 *)&D_800BCFFB_o)
#define D_800B1624_A (*(u8 **)&D_800B1624_a_o)
#define D_800B1624_B (*(u8 **)&D_800B1624_b_o)
#define D_800B1624_C (*(u8 **)&D_800B1624_c_o)
#define READ_S32(base, offset) (*(s32 *)((u8 *)(base) + (offset)))
#define READ_U16(base, offset) (*(u16 *)((u8 *)(base) + (offset)))
#define WRITE_U16(base, offset, value) (*(u16 *)((u8 *)(base) + (offset)) = (value))

int Render_StepFade(void) {
    u8 *geom;
    register u8 *entry asm("$4");
    PrimEntry *prim;
    s32 fade_step;
    int fade_value;
    int tint_loop;
    int entry_index;
    int prim_index;
    int entry_count;
    int prim_count;
    int active_slot;
    register s32 flags asm("$2");
    s32 mask;
    s32 divisor;
    u8 *final_geom;
    u8 *fade_ptr;
    u8 frame;
    s32 stack_pad[4];

    if ((D_800BCF88_WORD & 0x400) == 0) {
        return 0;
    }

    fade_step = D_800BCFFB_BYTE;
    divisor = D_800BCFFA_BYTE;
    fade_step <<= 7;
    divisor--;
    fade_step /= divisor;

    entry_index = 0;
    geom = D_800B1624_A;
    entry = D_800B1624_B;
    fade_value = READ_S32(geom, 0x14);
    entry_count = READ_U16(geom, 0x6);
    entry = entry + fade_value;
    fade_value = 0x80 - fade_step;
    if (entry_count != 0) {
        tint_loop = fade_value;
        do {
            prim = (PrimEntry *)READ_S32(entry, 0x30);
            active_slot = D_8009CDDC;
            prim_count = READ_U16(entry, 0x26);
            if (active_slot != 0) {
                prim = prim + prim_count;
            }
            prim_index = 0;
            if (prim_count != 0) {
                do {
                    prim->b = tint_loop;
                    prim->g = tint_loop;
                    prim->r = tint_loop;
                    asm("" : : : "memory");
                    prim_index++;
                    prim++;
                } while ((u32)prim_index < prim_count);
            }
            entry_index++;
            entry += 0x38;
        } while (entry_index < entry_count);
    }

    fade_ptr = D_800BCFFB_PTR;
    frame = *fade_ptr;
    frame++;
    *fade_ptr = frame;
    if (frame < D_800BCFFA_BYTE) {
        return 0;
    }

    flags = D_800BCF88_WORD;
    mask = -0xC01;
    flags &= mask;
    final_geom = D_800B1624_C;
    flags |= 0x800;
    *(s32 *)&D_800BCF88_store_o = flags;
    flags = 0x1FF0;
    WRITE_U16(final_geom, 0x26, flags);

    return 0;
}

int Render_BeginSceneLoad(void) {
    u32 *flags;
    u32 temp;
    u32 old;
    u32 value;
    u32 next;
    u32 mask;
    u32 *flags2;
    u32 value2;
    flags = (u32 *)&g_RenderStateFlags;
    old = *flags;
    value = old | 0x1000;
    *flags = value;
    if (value & 0x2000) {
        temp = value & ~0x2000;
        next = temp;
    } else {
        next = old | 0x3000;
    }
    *flags = next;
    asm volatile("" : : : "memory");
    mask = 0xFFFF0000;
    flags2 = (u32 *)&g_RenderStateFlags;
    value2 = *flags2;
    mask |= 0x3FFF;
    value2 &= mask;
    value2 |= 0x4000;
    *flags2 = value2;
    return 0;
}

extern struct { char _[16]; } D_800BCFFC_o __asm__("D_800BCFFC");
extern s32 g_ActiveDrawSlot;

#define D_800BCFFC_BYTE (*(u8 *)&D_800BCFFC_o)

int Render_ApplyScreenTint(void) {
    u8 *geom;
    register u8 *entry asm("$4");
    PrimEntry *prim;
    u32 flags;
    int tint;
    int tint_loop;
    int entry_index;
    int prim_index;
    int entry_count;
    int prim_count;
    int active_slot;
    s32 stack_pad[4];

    flags = D_800BCF88_WORD;
    if ((flags & 0x1000) == 0) {
        return 0;
    }

    if (flags & 0x2000) {
        tint = D_800BCFFC_BYTE;
    } else {
        tint = 0x80;
    }

    entry_index = 0;
    geom = D_800B1624_A;
    entry = D_800B1624_B;
    entry = entry + READ_S32(geom, 0x14);
    entry_count = READ_U16(geom, 0x6);
    if (entry_count != 0) {
        tint_loop = tint;
        do {
            prim = (PrimEntry *)READ_S32(entry, 0x30);
            active_slot = g_ActiveDrawSlot;
            prim_count = READ_U16(entry, 0x26);
            if (active_slot != 0) {
                prim = prim + prim_count;
            }
            prim_index = 0;
            if (prim_count != 0) {
                do {
                    prim->b = tint_loop;
                    prim->g = tint_loop;
                    prim->r = tint_loop;
                    asm("" : : : "memory");
                    prim_index++;
                    prim++;
                } while ((u32)prim_index < prim_count);
            }
            entry_index++;
            entry += 0x38;
        } while (entry_index < entry_count);
    }

    {
        s32 mask;
        s32 *flags_ptr;
        register s32 old_flags asm("$2");
        s32 mode_bits;
        register s32 value asm("$2");

        mask = 0xFFFF3FFF;
        flags_ptr = &D_800BCF88_WORD;
        old_flags = *flags_ptr;
        mask = old_flags & mask;
        mode_bits = old_flags & 0xC000;
        value = 0x4000;
        *flags_ptr = mask;
        if (mode_bits == value) {
            goto mode_4000;
        }
        value = 0x8000;
        if (mode_bits == value) {
            goto mode_8000;
        }
        return 0;
mode_4000:
        value = mask | 0x8000;
        goto store_flags;
mode_8000:
        value = -0x1001;
        value = mask & value;
store_flags:
        *flags_ptr = value;
    }

    return 0;
}

/* Historical name: applies texture scrolling and camera-relative parallax. */
int Scene_IsBattleMode(void)
{
    if (!(g_GameStateFlags & 0x104)) {
        int i;
        GeomState *state = D_800B1624;
        GeomStateAddress table;
        GeomEntryView *entries;
        int count;
        GeomScrollEntry *entry;

        table.state = D_800B1624;
        table.word += state->entry_offset;
        entries = table.views;
        count = state->entry_count06;

        i = 0;
        if (i < count) {
            GeomScrollCoordinates *camera = &D_800BCF8C;

            entry = &entries->scroll;
            do {
            int position;

            if (entry->flags & 4) {
                int x, y, fracX;

                position = (entry->x * 256) | entry->fractionX.byte;
                position += entry->speedX;
                x = (position >> 8) % entry->modulusX;
                fracX = position & 255;
                position = (entry->y * 256) | entry->fractionY.byte;
                position += entry->speedY;
                y = (position >> 8) % entry->modulusY;
                entry->fractionX.word = fracX;
                entry->fractionY.word = position & 255;
                entry->x = x;
                entry->y = y;
            }
            if (entry->flags & 8) {
                /* The final fixed-point sum wraps at the target word width. */
                position = (unsigned int)(entry->baseX * 256) +
                    (camera->x - camera->originX) * entry->speedX;
                entry->x = position >> 8;
                entry->fractionX.word = position & 255;
                position = (unsigned int)(entry->baseY * 256) +
                    (camera->y - camera->originY) * entry->speedY;
                entry->y = position >> 8;
                entry->fractionY.word = position & 255;
            }
            entry++;
            } while (++i < count);
        }
        if (D_800BCF88.state.flags & 0x80) {
            unsigned short x = D_800BCF8C.x, y = D_800BCF8E;
            D_800BCF88.state.flags &= ~0x80;
            D_800BCF90 = x;
            D_800BCF92 = y;
        }
    }
    return 0;
}

/* Builds the ground-aligned shadow transform for `actor` and links its
 * shadow quad, projected from a square of the model's shadow radius.
 * Matching debt: the remaining empty constraints. Rotation, column, and
 * translation windows use the stock GTE macros. */
int Render_DrawRoom(RenderShadowActor *actor)
{
    GteShortVector origin;
    GteShortVector corner;
    GteVector axis;
    GteVector normal;
    s32 normalX, normalY, normalZ;
    GteVector up;
    GteMatrixStorage local;
    GteMatrix world;
    u16 *firstColumn;
    s32 sxy;
    s32 p;
    s32 flag;
    RenderShadowQuad *quad;
    GteMatrixStorage *source;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    int radius;
    int shade;
    int depth;
    int maxDepth;
    u32 last;
    const GteMatrixWords *matrix;
    u32 first; /* word 0 is read before the rest of the copy */

    if (actor->flags & 0x400)
        return 0;
    if (!actor->visible)
        return 0;

    radius = actor->header->shadow_radius;
    origin.x = actor->x.integer;
    origin.y = actor->y.integer;
    origin.z = actor->z.integer;
    source = &actor->matrices[actor->shadow_matrix_index];
    first = source->words[0];
    local.words[1] = source->words[1];
    local.words[2] = source->words[2];
    local.words[3] = source->words[3];
    local.words[4] = source->words[4];
    local.words[5] = source->words[5];
    local.words[6] = source->words[6];
    last = source->words[7];
    corner.z = 0x1000;
    asm volatile("" : : "m"(corner.z), "r"(last));
    matrix = (const GteMatrixWords *)local.words;
    local.words[0] = first;
    corner.x = 0;
    corner.y = 0;
    local.matrix.t[1] = 0;
    local.matrix.t[0] = 0;
    /* Retain the final word of the retail copy before clearing translation. */
    *(volatile u32 *)&local.words[7] = last;
    local.matrix.t[2] = 0;
    {
                gte_ldrotmatrix(matrix);
        gte_ldtransmatrix(matrix);
    }
    gte_lwc2_0_0(&corner);
    gte_lwc2_1_4(&corner);
    gte_rt();
    gte_swc2_25_0(&axis);
    gte_swc2_26_4(&axis);
    gte_swc2_27_8(&axis);
    axis.y = 0;
    if (axis.x == 0 && axis.z == 0) {
        normal.x = 0;
        normal.y = 0;
        normal.z = 0x1000;
    } else {
        VectorNormal(&axis, &normal);
    }
    normalX = normal.x;
    normalY = normal.y;
    normalZ = normal.z;
    local.matrix.m[1][1] = 0x1000;
    up.y = 0x1000;
    asm volatile("" : : "r"(normalX), "r"(normalY), "r"(normalZ), "m"(up.y));
    local.matrix.m[0][1] = 0;
    local.matrix.m[2][1] = 0;
    up.x = 0;
    up.z = 0;
    local.matrix.m[0][2] = normalX;
    local.matrix.m[1][2] = normalY;
    local.matrix.m[2][2] = normalZ;
    {
        const GteVector *vector = &normal;
        asm volatile("" : "=r"(vector) : "0"(vector));
        gte_ldopv1_psyq(vector);
    }
    gte_ldir3_precise(&up);
    gte_ldir1_precise(&up);
    gte_ldir2_precise(&up);
    gte_op12_psyq();
    gte_swc2_25_0(&axis);
    gte_swc2_26_4(&axis);
    gte_swc2_27_8(&axis);
    local.matrix.m[0][0] = axis.x;
    local.matrix.m[1][0] = axis.y;
    local.matrix.m[2][0] = axis.z;
    if (actor->flags & 0x4000000)
        local.matrix.t[1] = g_RoomFloorY->y;
    else
        local.matrix.t[1] = origin.y;
    local.matrix.t[0] = actor->matrices[actor->shadow_matrix_index].matrix.t[0];
    local.matrix.t[2] = actor->matrices[actor->shadow_matrix_index].matrix.t[2];
    {
        const GteMatrixWords *cameraRot;
        const GteMatrixWords *cameraTrans;
        const u16 *column;
        u16 *outColumn;

        s32 *outTranslation;
        const s32 *translation;
        cameraRot = (const GteMatrixWords *)D_800B89F8;
        /* Keep the camera address in t0 without pinning it: GCC must also
         * be able to reuse t0 for the signed division below. */
        asm volatile("" : : : "$3", "$4", "$5", "$6", "$7");
        asm volatile("" : "=r"(cameraRot) : "0"(cameraRot));

        gte_ldrotmatrix(cameraRot);
        column = (const u16 *)&local.matrix;
                gte_ldclmv(column);
        gte_rtir();
        firstColumn = (u16 *)&world;
                gte_stclmv(firstColumn);
                column = (const u16 *)&local.matrix + 1;
                gte_ldclmv(column);
        gte_rtir();
        outColumn = (u16 *)&world + 1;
                gte_stclmv(outColumn);
                column = (const u16 *)&local.matrix + 2;
                gte_ldclmv(column);
        gte_rtir();
        outColumn = (u16 *)&world + 2;
                gte_stclmv(outColumn);
        /* Transform placement translation with the camera matrix. */
                cameraTrans = cameraRot;

        gte_ldtransmatrix(cameraTrans);
        translation = local.matrix.t;
                gte_ldlv0(translation);
        gte_rt();
        outTranslation = world.t;
        gte_swc2_9_0(outTranslation);
        gte_swc2_10_4(outTranslation);
        gte_swc2_11_8(outTranslation);
    }
    SetRotMatrix((GteMatrix *)firstColumn);
    SetTransMatrix((GteMatrix *)firstColumn);
    SetGeomScreen(D_800B89F8[8]);

    /* Exclude unused temporaries from GCC's division reload scratch choice. */
    asm volatile("" : : : "$9", "$10", "$11", "$15", "$24", "$25");
    quad = &actor->shadow_quads[D_8009CDDC];
    quad->tag.length = 9;
    quad->code = 0x2C;
    if (actor->render_flags & 6)
        shade = (actor->fade_red + actor->fade_green + actor->fade_blue) / 3;
    else
        shade = 0x80;
    shade = shade * actor->shadow_intensity / 128;
    {
        int half = shade / 2;

        quad->r0 = half;
        quad->g0 = half;
        quad->b0 = half;
    }
    quad->u0 = 0;
    quad->v0 = 0x40;
    quad->u1 = 0x3F;
    quad->v1 = 0x40;
    quad->u2 = 0;
    quad->v2 = 0x7F;
    quad->u3 = 0x3F;
    quad->v3 = 0x7F;
    quad->clut = 0x7210;
    quad->tpage = 0xCB;
    quad->code |= 2;

    maxDepth = -1;
    corner.y = 0;
    corner.x = -radius;
    corner.z = radius;
    depth = RotTransPers(&corner, &sxy, &p, &flag);
    quad->xy0 = sxy;
    if (maxDepth < depth)
        maxDepth = depth;
    corner.x = radius;
    corner.z = radius;
    depth = RotTransPers(&corner, &sxy, &p, &flag);
    quad->xy1 = sxy;
    if (maxDepth < depth)
        maxDepth = depth;
    corner.x = -radius;
    corner.z = -radius;
    depth = RotTransPers(&corner, &sxy, &p, &flag);
    quad->xy2 = sxy;
    if (maxDepth < depth)
        maxDepth = depth;
    corner.x = radius;
    corner.z = -radius;
    depth = RotTransPers(&corner, &sxy, &p, &flag);
    quad->xy3 = sxy;
    if (maxDepth < depth)
        maxDepth = depth;

    if (maxDepth >= 0 && maxDepth < 0x1000) {
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], maxDepth);
        quad->tag.address = ot.tag->address;
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], maxDepth);
        link.tag = &quad->tag;
        ot.tag->address = link.word;
    }
    /* These registers are already saved by the function. Excluding them
     * from reload leaves t0 available for the multiply-high temporary. */
        return 0;
}

void Render_SetPrimColour(PrimObj *obj, unsigned char a, unsigned char b, unsigned char c) {
    int i;
    int offset;
    unsigned char *base;

    for (i = 0; i < obj->count; i++) {
        offset = i << 4;
        base = (unsigned char *) obj->entries;
        base[offset + 4] = a;
        base[offset + 5] = b;
        base[offset + 6] = c;

        base = (unsigned char *) obj->entries + (obj->count << 4);
        base[offset + 4] = a;
        base[offset + 5] = b;
        base[offset + 6] = c;
    }
}
