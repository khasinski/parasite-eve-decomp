/* CC1_FLAGS: -fno-strength-reduce */

/* Geometry sprite entries and mesh lists, the screen fade and tint, the
 * background scroll/parallax step and the room shadow and primitive colour
 * draws (G0833/G0835). */

#include "common.h"
#include "pe1/field_tile.h"
#include "pe1/fieldfx_task.h"
#include "pe1/game_state.h"
#include "pe1/geom_state.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_nop.h"
#include "pe1/render_buffers.h"
#include "pe1/render_camera.h"
#include "pe1/render_prim.h"
#include "pe1/render_shadow.h"

/* Matching debt: fixed-register pins and scheduling constraints preserve the stock PSYQ instruction schedule. */

/* A packet or ordering-table slot address as the 24-bit link word. */
typedef union RenderLinkAddress {
    void *pointer;
    u8 *bytes;
    u32 *words;
    GeomEntry *entry;
    u32 word;
} RenderLinkAddress;

#define LINK_PACKET_PRELOADED(packet, ordering, packet_tag, mask24, maskTop) \
    do {                                                                    \
        u32 ot_tag;                                      \
        RenderLinkAddress link;                                             \
        ot_tag = *(u32 *)(ordering);                                        \
        *(u32 *)(packet) = ((packet_tag) & (maskTop)) | (ot_tag & (mask24)); \
        ot_tag = *(u32 *)(ordering);                                        \
        link.pointer = (packet);                                            \
        *(u32 *)(ordering) = (ot_tag & (maskTop)) | (link.word & (mask24)); \
    } while (0)

#define LINK_PAGE(packet, ordering, mask24, maskTop)                         \
    do {                                                                     \
        u32 packet_tag = *(u32 *)(packet);               \
        register u32 ot_tag asm("$2");                                      \
        RenderLinkAddress link;                                              \
        ot_tag = *(u32 *)(ordering);                                         \
        ot_tag &= (mask24);                                                   \
        packet_tag &= (maskTop);                                              \
        packet_tag |= ot_tag;                                                 \
        *(u32 *)(packet) = packet_tag;                                        \
        ot_tag = *(u32 *)(ordering);                                         \
        link.pointer = (packet);                                             \
        *(u32 *)(ordering) = (ot_tag & (maskTop)) | (link.word & (mask24)); \
    } while (0)

int Render_DrawSpriteEntry(GeomEntry *input)
{
    register u32 *position_cursor asm("$10");
    register s32 workValue asm("$2");
    GeomEntry *entry;
    s32 active;
    RenderTilePacket *sprite;
    register RenderTexturePagePacket *page asm("$8");
    RenderLinkAddress ordering;
    GeomState *state;
    u32 flags;
    u32 count;
    u32 i;
    register s32 scroll_x asm("$6");
    s32 scroll_y;
    s32 texture_base;
    register u32 *pos asm("$25");
    s32 raw_x;
    register s32 raw_y asm("$7");
    register s32 screen_src asm("$3");
    char frame_pad[9];

    active = g_ActiveDrawSlot;
    entry = input;
    sprite = entry->u30.prim;
    count = entry->prim_count;
    asm("" : "=r"(active), "=r"(sprite), "=r"(count)
        : "0"(active), "1"(sprite), "2"(count));
    ordering.pointer = D_800B08F8.ordering[active];
    if (active != 0) {
        sprite += count;
    }
    page = entry->u34.pagePackets;
    if (active != 0)
        page += count;

    state = g_GeomState;
    {
        s32 screen_add;
        screen_src = entry->scr_x;
        screen_add = (u16)state->out_disp_x;
        raw_x = screen_src + screen_add;
        screen_src = entry->scr_y;
        screen_add = (u16)state->out_disp_y;
        scroll_x = raw_x;
        raw_y = screen_src + screen_add;
        scroll_y = raw_y;
    }
    asm volatile("" : : "r"(scroll_y));
    {
        workValue = *(u32 *)entry >> 8;
        workValue &= 0xFFF;
        screen_src = state->field26;
        texture_base = screen_src + workValue;
    }
    screen_src = entry->u28.pos_ptr;
    flags = entry->head.b.flags;
    {
        /* The position words lie pos_ptr bytes past the entry. */
        RenderLinkAddress positions;

        positions.entry = entry;
        positions.word += screen_src;
        pos = positions.words;
    }

    if (flags & 4) {
        s32 mod_x;
        s32 mod_y;
        s32 divisor_x;
        s32 remainder_y;
        s32 signed_x;
        s32 base_y;
        signed_x = (s16)raw_x;
        mod_x = entry->anim_mod_x;
        signed_x -= 320;
        divisor_x = mod_x & 0xFFFF;
        signed_x += divisor_x;
        scroll_x = signed_x % divisor_x;
        mod_y = entry->anim_mod_y;
        workValue = (s16)raw_y;
        workValue -= 224;
        workValue += mod_y & 0xFFFF;
        remainder_y = workValue % (mod_y & 0xFFFF);
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
                        s16 draw_y;
                        {
                            u32 word_y = *position_cursor;
                            s32 y = scroll_y + ((word_y >> 12) & 0x3FF);
                            draw_y = y;
                            if ((s16)y >= 224) {
                                workValue = entry->anim_mod_y;
                                draw_y = y - workValue;
                            } else if ((s16)y < -15) {
                                workValue = entry->anim_mod_y;
                                draw_y = workValue + y;
                            }
                        }
                        if ((u16)(draw_y + 15) < 0xEF) {
                            s32 tile_u = texture_base + (*(u16 *)position_cursor & 0xFFF);
                            if (((tile_u - 8) & 0xFFFFU) < 0xFF1) {
                                u32 sprite_tag;
                                s32 ot_index = tile_u;
                                RenderLinkAddress ot;
                                ot_index <<= 16;
                                ot_index >>= 14;
                                ot.word = ot_index + ordering.word;
                                sprite_tag = *(u32 *)sprite_cursor;
                                (sprite_cursor)->x = draw_x;
                                (sprite_cursor)->y = draw_y;
                                LINK_PACKET_PRELOADED(sprite_cursor, ot.bytes, sprite_tag, mask24, maskTop);
                                LINK_PAGE(page_cursor, ot.bytes, mask24, maskTop);
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
        i = 0;
        if (count != 0) {
            mask24 = 0x00FFFFFF;
            maskTop = 0xFF000000;
            sprite_cursor = sprite;
            page_cursor = page;
            position_cursor = pos;
            do {
            u32 word = *position_cursor;
            register s32 draw_x asm("$13");
            workValue = scroll_x + (word >> 22);
            draw_x = workValue;
            if ((u16)(workValue + 15) < 0x14F) {
                    {
                    s32 y;
                    s32 draw_y;
                    s32 tile_u;
                    y = scroll_y + ((word >> 12) & 0x3FF);
                    draw_y = y;
                    if ((u16)(y + 15) < 0xEF) {
                        tile_u = texture_base + (*(u16 *)position_cursor & 0xFFF);
                        if (((tile_u - 8) & 0xFFFFU) < 0xFF1) {
                            register u32 sprite_tag asm("$3");
                            s32 ot_index = tile_u;
                            RenderLinkAddress ot;
                            ot_index <<= 16;
                            ot_index >>= 14;
                            ot.word = ot_index + ordering.word;
                            sprite_tag = *(u32 *)sprite_cursor;
                            (sprite_cursor)->x = draw_x;
                            (sprite_cursor)->y = draw_y;
                            LINK_PACKET_PRELOADED(sprite_cursor, ot.bytes, sprite_tag, mask24, maskTop);
                            LINK_PAGE(page_cursor, ot.bytes, mask24, maskTop);
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
        entry->head.b.flags |= 2;
    } else {
        entry->head.b.flags &= 0xFD;
    }
    return 0;
}

int Render_SetEntryScrolled(int index, int enabled, unsigned int arg2, unsigned int arg3) {
    GeomStateAddress table, base;
    GeomEntry *entry = (GEOM_STATE_OFFSET(table, base, entry_offset, 0), table.entry) + index;

    if (enabled != 0) {
        entry->head.b.flags |= 4;
    } else {
        entry->head.b.flags &= 0xFB;
    }
    entry->speedX = arg2 >> 8;
    entry->speedY = arg3 >> 8;
    return 0;
}

int Render_SetEntryMirrored(int index, int enabled, unsigned int arg2, unsigned int arg3) {
    GeomStateAddress table, base;
    GeomEntry *entry = (GEOM_STATE_OFFSET(table, base, entry_offset, 0), table.entry) + index;

    if (enabled != 0) {
        entry->head.b.flags |= 8;
    } else {
        entry->head.b.flags &= 0xF7;
    }
    entry->speedX = (0x10000 - arg2) >> 8;
    entry->speedY = (0x10000 - arg3) >> 8;
    return 0;
}

int Render_SetEntryPosition(int index, int x, int y) {
    GeomStateAddress table, base;
    GeomEntry *entry = (GEOM_STATE_OFFSET(table, base, entry_offset, 0), table.entry) + index;
    int *flags = &g_RenderStateFlags;

    entry->scr_x = x;
    entry->base_x = x;
    entry->scr_y = y;
    entry->base_y = y;
    *flags |= 0x80;
    return 0;
}

int Render_LoadGeomState(int);
/* Loads the selected group's viewport, centres the camera on it and builds
 * every mesh entry's packets into `buffer`; *end returns the free space. */
int Render_DrawMeshList(void *buffer, void **end)
{
    GeomState *state = g_GeomState;
    u8 *group = &g_GeomGroupSel;
    GeomStateAddress address;
    CameraViewport *view;
    GeomEntry *entry;
    unsigned int count;
    unsigned int i;
    int x;

    Render_LoadGeomState(*group);
    address.state = state;
    address.word += state->entry_offset_1C;
    view = &address.viewport[*group];
    x = (view->minX + view->maxX) / 2;
    g_GeomScroll.state.position.x = g_GeomScroll.state.position.originX = x;
    g_GeomScroll.state.position.y = g_GeomScroll.state.position.originY =
        (view->minY + view->maxY) / 2;
    g_GeomScroll.state.position.boundsMinX = view->minX;
    g_GeomScroll.state.position.boundsMaxX = view->maxX;
    i = 0;
    g_GeomScroll.state.position.boundsMinY = view->minY;
    g_GeomScroll.state.position.boundsMaxY = view->maxY;
    x -= 160;
    count = state->entry_count06;
    state->out_disp_x = state->disp_src_x - x;
    state->out_disp_y = state->disp_src_y - ((u16)g_GeomScroll.state.position.y - 112);
    address.state = state;
    address.word += state->entry_offset;
    *end = buffer;
    if (i < count) {
        int min = -32768, max = 32767;

        entry = address.entry;
        do {
            if (Render_LoadMeshEntry(entry, *end, end))
                return -18;
            entry->bounds.minX = min;
            entry->bounds.maxX = max;
            entry->bounds.minY = min;
            entry->bounds.maxY = max;
            entry++;
        } while (++i < count);
    }
    {
        int *flags = &g_RenderStateFlags;

        g_RenderFadeFrameCount = g_RenderFadeFrameCounter = 0;
        *flags &= ~0xC00;
    }
    return 0;
}

/* Field geometry: clamp the display-source point into the clip window and
 * draw the selected group's sprite entries relative to the camera.
 * Contiguous default-profile pair on g_GeomState (main_tu_evidence G0836
 * joins them with the mesh renderer before and the fades after). */

/* Offsets a point by the display origin and clamps it into the clip
 * window; the result becomes the display source point and depth. */
int Render_ClipPoint(int x, int y, int z) {
    GeomState *state = g_GeomState;
    u16 clippedX = state->clip_offset_x + x;
    u16 clippedY = state->clip_offset_y + y;
    int depth = state->depth_offset + z;

    if ((s16)clippedX < state->clip_min_x)
        clippedX = state->clip_min_x;
    else if (state->clip_max_x < (s16)clippedX)
        clippedX = state->clip_max_x;
    if ((s16)clippedY < state->clip_min_y)
        clippedY = state->clip_min_y;
    else if (state->clip_max_y < (s16)clippedY)
        clippedY = state->clip_max_y;
    state->disp_src_x = clippedX;
    state->disp_src_y = clippedY;
    state->field26 = depth;
    return 0;
}

/* Re-derives the display offsets from the camera and draws the sprite
 * entries of the selected group. */
s32 Render_BuildMeshList(void) {
    GeomStateAddress base;
    GeomEntry *entry;
    s32 count;
    s32 i;

    base.state = g_GeomState;
    base.state->out_disp_x = base.state->disp_src_x - ((u16)g_GeomScroll.state.position.x - 160);
    count = base.state->entry_count06;
    base.state->out_disp_y = base.state->disp_src_y - ((u16)g_GeomScroll.state.position.y - 112);
    base.word += base.state->entry_offset;
    i = 0;
    if (i < count) {
        entry = base.entry;
        do {
            if ((entry->head.b.flags & 2) && entry->group == g_GeomGroupSel)
                Render_DrawSpriteEntry(entry);
            i++;
            entry++;
        } while (i < count);
    }
    return 0;
}

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

/* Steps the screen fade: every sprite primitive of every entry gets the
 * current grey level; when the fade has run its frames the fade-in mode is
 * switched off and the fade-out flag set. */
int Render_StepFade(void) {
    GeomState *state;
    GeomStateAddress entries;
    GeomEntry *entry;
    u8 *counter;
    int level;
    int count;
    int i;

    if (!(g_RenderStateFlags & 0x400))
        return 0;
    level = (g_RenderFadeFrameCounter << 7) / (g_RenderFadeFrameCount - 1);
    state = g_GeomState;
    entries.state = g_GeomState;
    entries.word += state->entry_offset;
    count = state->entry_count06;
    level = 0x80 - level;
    entry = entries.entry;
    for (i = 0; i < count; i++, entry++) {
        PrimEntry *prim = entry->u30.prim;
        u32 primCount = entry->prim_count;
        u8 grey = level;
        u32 j;

        if (g_ActiveDrawSlot != 0)
            prim += primCount;
        for (j = 0; j < primCount; j++) {
            prim->b = grey;
            prim->g = grey;
            prim->r = grey;
            prim++;
        }
    }
    counter = &g_RenderFadeFrameCounter;
    if (++*counter >= g_RenderFadeFrameCount) {
        g_RenderStateFlags = (g_RenderStateFlags & ~0xC00) | 0x800;
        g_GeomState->field26 = 0x1FF0;
    }
    return 0;
}

/* Starts the scene-load fade: sets the load flag (toggling the 0x2000
 * phase bit) and selects tint mode 1. */
int Render_BeginSceneLoad(void) {
    {
        int *flags = &g_RenderStateFlags;
        int old = *flags;
        int value = old | 0x1000;

        *flags = value;
        if (value & 0x2000)
            *flags = value & ~0x2000;
        else
            *flags = old | 0x3000;
    }
    {
        int *flags = &g_RenderStateFlags;

        *flags = (*flags & ~0xC000) | 0x4000;
    }
    return 0;
}

/* Tints every sprite primitive with the tint level (or neutral grey while
 * the tint is off) and advances the two-step tint mode. */
int Render_ApplyScreenTint(void) {
    GeomState *state;
    GeomStateAddress entries;
    GeomEntry *entry;
    int *flags;
    int mode;
    int masked;
    int level;
    int count;
    int i;

    if (!(g_RenderStateFlags & 0x1000))
        return 0;
    level = 0x80;
    if (g_RenderStateFlags & 0x2000)
        level = g_RenderMeshTintLevel;
    state = g_GeomState;
    entries.state = g_GeomState;
    entries.word += state->entry_offset;
    count = state->entry_count06;
    entry = entries.entry;
    for (i = 0; i < count; i++, entry++) {
        PrimEntry *prim = entry->u30.prim;
        u32 primCount = entry->prim_count;
        u8 grey = level;
        u32 j;

        if (g_ActiveDrawSlot != 0)
            prim += primCount;
        for (j = 0; j < primCount; j++) {
            prim->b = grey;
            prim->g = grey;
            prim->r = grey;
            prim++;
        }
    }
    flags = &g_RenderStateFlags;
    mode = *flags;
    masked = mode & ~0xC000;
    *flags = masked;
    switch (mode & 0xC000) {
    case 0x4000:
        *flags = masked | 0x8000;
        break;
    case 0x8000:
        *flags = masked & ~0x1000;
        break;
    default:
        return 0;
    }
    return 0;
}

/* Applies texture scrolling and camera-relative parallax to the entries. */
int Render_ScrollEntries(void)
{
    if (!(g_GameStateFlags & 0x104)) {
        int i;
        GeomState *state = g_GeomState;
        GeomStateAddress table;
        GeomEntryView *entries;
        int count;
        GeomScrollEntry *entry;

        table.state = g_GeomState;
        table.word += state->entry_offset;
        entries = table.views;
        count = state->entry_count06;

        i = 0;
        if (i < count) {
            GeomScrollCoordinates *camera = &g_GeomScroll.state.position;

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
                    position = entry->baseX * 256 +
                        (camera->x - camera->originX) * entry->speedX;
                    entry->x = position >> 8;
                    entry->fractionX.word = position & 255;
                    position = entry->baseY * 256 +
                        (camera->y - camera->originY) * entry->speedY;
                    entry->y = position >> 8;
                    entry->fractionY.word = position & 255;
                }
                entry++;
            } while (++i < count);
        }
        if (g_GeomScroll.state.flags & 0x80) {
            unsigned short x = g_GeomScroll.state.position.x, y = g_GeomScroll.state.position.y;
            g_GeomScroll.state.flags &= ~0x80;
            g_GeomScroll.state.position.savedX = x;
            g_GeomScroll.state.position.savedY = y;
        }
    }
    return 0;
}

/* Builds the ground-aligned shadow transform for `actor` and links its
 * shadow quad, projected from a square of the model's shadow radius. */
int Render_DrawActorShadow(RenderShadowActor *actor)
{
    GteShortVector origin;
    GteShortVector corner;
    GteVector axis;
    GteVector normal;
    GteVector up;
    GteMatrixStorage local;
    GteMatrix world;
    GteMatrixStorage *source;
    u32 first, last;
    s32 sxy;
    s32 p;
    s32 flag;
    RenderShadowQuad *quad;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    int radius;
    int shade;
    int depth;
    int maxDepth;

    if (actor->flags & 0x400)
        return 0;
    if (!actor->visible)
        return 0;

    radius = actor->header->shadow_radius;
    origin.x = actor->x.integer;
    origin.y = actor->y.integer;
    origin.z = actor->z.integer;
    /* The part matrix without its translation turns +Z into the facing. */
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
    local.words[0] = first;
    corner.x = 0;
    corner.y = 0;
    local.matrix.t[1] = 0;
    local.matrix.t[0] = 0;
    /* Matching debt: retail keeps this store although t[2] overwrites it. */
    *(volatile u32 *)&local.words[7] = last;
    local.matrix.t[2] = 0;
    gte_ldrotmatrix(&local);
    gte_ldtransmatrix(&local);
    gte_ldv0(&corner);
    gte_rt();
    gte_stlvnl(&axis);
    axis.y = 0;
    if (axis.x == 0 && axis.z == 0) {
        normal.x = 0;
        normal.y = 0;
        normal.z = 0x1000;
    } else {
        VectorNormal(&axis, &normal);
    }
    /* Shadow basis: X = up x facing, Y = up, Z = facing. */
    local.matrix.m[1][1] = 0x1000;
    up.y = 0x1000;
    local.matrix.m[0][1] = 0;
    local.matrix.m[2][1] = 0;
    up.x = 0;
    up.z = 0;
    local.matrix.m[0][2] = normal.x;
    local.matrix.m[1][2] = normal.y;
    local.matrix.m[2][2] = normal.z;
    gte_ldopv1_psyq(&normal);
    gte_ldopv2(&up);
    gte_op12_psyq();
    gte_stlvnl(&axis);
    local.matrix.m[0][0] = axis.x;
    local.matrix.m[1][0] = axis.y;
    local.matrix.m[2][0] = axis.z;
    if (actor->flags & 0x4000000)
        local.matrix.t[1] = g_RoomFloorY->y;
    else
        local.matrix.t[1] = origin.y;
    local.matrix.t[0] = actor->matrices[actor->shadow_matrix_index].matrix.t[0];
    local.matrix.t[2] = actor->matrices[actor->shadow_matrix_index].matrix.t[2];
    gte_CompMatrix((GteMatrixWords *)g_EntityRenderScratch, &local, &world);
    SetRotMatrix(&world);
    SetTransMatrix(&world);
    SetGeomScreen(g_EntityRenderScratch[8]);

    quad = &actor->shadow_quads[g_ActiveDrawSlot];
    quad->tag.length = 9;
    quad->code = 0x2C;
    if (actor->render_flags & 6)
        shade = (actor->fade_red + actor->fade_green + actor->fade_blue) / 3;
    else
        shade = 0x80;
    shade = shade * actor->shadow_intensity / 128;
    quad->r0 = shade / 2;
    quad->g0 = shade / 2;
    quad->b0 = shade / 2;
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
        TILE_OT_ENTRY(ot, table, D_800B08F8.ordering[g_ActiveDrawSlot], maxDepth);
        quad->tag.address = ot.tag->address;
        TILE_OT_ENTRY(ot, table, D_800B08F8.ordering[g_ActiveDrawSlot], maxDepth);
        link.tag = &quad->tag;
        ot.tag->address = link.word;
    }
    return 0;
}

/* Sets the colour of every primitive in both halves of the double-buffered
 * primitive list. */
void Render_SetPrimColour(PrimObj *obj, unsigned char r, unsigned char g, unsigned char b) {
    int i;

    for (i = 0; i < obj->count; i++) {
        PrimEntry *entries = obj->entries;

        entries[i].r = r;
        entries[i].g = g;
        entries[i].b = b;
        entries = obj->entries + obj->count;
        entries[i].r = r;
        entries[i].g = g;
        entries[i].b = b;
    }
}
