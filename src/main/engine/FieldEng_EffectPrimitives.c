/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/field_rotated_triangle.h"
#include "pe1/field_history_trail.h"
#include "pe1/field_tile.h"
#include "pe1/geom_state.h"
#include "pe1/field_rotated_quad.h"

/* Triangle with its apex at `position` and a `breadth` wide base `length`
 * away along local Z, turned by `rotation` and shaded per corner from
 * color0..color2 at intensity/128; mode 0xFF draws it opaque, anything
 * else adds a semi-transparent draw mode.
 * Matching debt: six register pins and two empty constraints. The camera
 * and local matrix uploads are gte_ldrotmatrix and gte_ldtransmatrix. */
void func_800D0E88(GteShortVector *position, GteRotation *rotation, int length,
                   int breadth, RenderColor *color0, RenderColor *color1,
                   RenderColor *color2, int intensity, int mode)
{
    GteShortVector corners[3];
    RenderColor black;
    GteMatrix matrix;
    u32 depth;
    FieldG3Packet *packet;
    RenderTintMode *drawMode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    GteMatrix *view;
    int bias;

    black = D_800C22A0;
    bias = (u16)D_800F3374;
    /* Keep intensity in s6 without pinning the later draw-mode lifetime. */
    asm("" : : "r"(intensity));

    packet = (FieldG3Packet *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(FieldG3Packet);
    view = D_800BCFA4.value;
    {
        const GteMatrixWords *words = (const GteMatrixWords *)(view);
        gte_ldrotmatrix(words);
        gte_ldtransmatrix(words);
    }
    gte_lwc2_0_0(position);
    gte_lwc2_1_4(position);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    corners[1].x = -breadth / 2;
    corners[1].y = corners[2].y = 0;
    corners[0].x = corners[0].y = corners[0].z = 0;
    corners[1].z = corners[2].z = length;
    corners[2].x = breadth / 2;
    {
        s32 *translation = matrix.t;
        gte_swc2_25_0(translation);
        gte_swc2_26_4(translation);
        gte_swc2_27_8(translation);
    }
    RotMatrixYXZ((GteShortVector *)rotation, &matrix);
    MulRotMatrix(&matrix);
    {
        register const GteMatrixWords *words asm("$16") = (const GteMatrixWords *)(&matrix);
        gte_ldrotmatrix(words);
        gte_ldtransmatrix(words);
    }
    {
        GteShortVector *v0;
        register GteShortVector *v1 asm("$4");
        GteShortVector *v2;
        v1 = &corners[1];
        v2 = &corners[2];
        v0 = &corners[0];
        gte_lwc2_0_0(v0);
        gte_lwc2_1_4(v0);
        gte_lwc2_2_0(v1);
        gte_lwc2_3_4(v1);
        gte_lwc2_4_0(v2);
        gte_lwc2_5_4(v2);
    }
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtpt_command();
    SetPolyG3(packet);
    if (color0 == 0) {
        color0 = &black;
    }
    packet->r0 = color0->r * (s16)intensity / 128;
    packet->g0 = color0->g * (s16)intensity / 128;
    packet->b0 = color0->b * (s16)intensity / 128;
    if (color1 == 0) {
        color1 = &black;
    }
    packet->r1 = color1->r * (s16)intensity / 128;
    packet->g1 = color1->g * (s16)intensity / 128;
    packet->b1 = color1->b * (s16)intensity / 128;
    if (color2 == 0) {
        color2 = &black;
    }
    packet->r2 = color2->r * (s16)intensity / 128;
    packet->g2 = color2->g * (s16)intensity / 128;
    packet->b2 = color2->b * (s16)intensity / 128;
    gte_stmac0(&depth);
    if (depth != 0) {
        {
            register s16 *xy0 asm("$4") = &packet->x0;
            register s16 *xy1 asm("$3") = &packet->x1;
            s16 *xy2 = &packet->x2;
            gte_stsxy0_precise(xy0);
            gte_stsxy1_precise(xy1);
            gte_stsxy2_precise(xy2);
        }
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_avsz3_command();
        {
            register s32 z asm("$12");
            register u32 *out asm("$5") = &depth;
            asm volatile("" : "=r"(out) : "0"(out));
            gte_getsz3(z);
            gte_cop2_hazard_slot();
            *out = z >> 2;
        }
        depth -= bias;
        if (depth < 0x1000) {
            TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], depth);
            if (mode != 0xFF) {
                drawMode = (RenderTintMode *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
                D_8009CDD8 += sizeof(RenderTintMode);
                SetDrawTPage((char *)drawMode, 0, 1, GetTPage(0, mode, 0, 0));
                if (packet) {
                    packet->code |= 2;
                    TILE_OT_ADDPRIM(ot, packet, link);
                }
                TILE_OT_ADDPRIM(ot, drawMode, link);
            } else if (packet) {
                TILE_OT_ADDPRIM(ot, packet, link);
            }
        }
    }
}

/* Push the pair (head, tail) onto the `count`-deep history and draw the
 * ribbon through it: quad i joins pairs i and i+1 and fades from
 * (count - i)/count to (count - i - 1)/count of color0 (head side) and
 * color1 (tail side) at scale/128. A missing head repeats the previous
 * pair; count >= 1000 instead clears the first count - 999 pairs. Mode
 * 0xFF draws opaque, anything else semi-transparent.
 * Matching debt: five register pins and one empty pointer constraint. The
 * camera upload is gte_ldrotmatrix and gte_ldtransmatrix. */
void func_800D1384(GteShortVector *head, GteShortVector *tail, u32 countArg,
                   u8 *color0, u8 *color1, int scale, FieldTrailPair *history,
                   int mode)
{
    register u32 count asm("$23") = countArg;
    u8 headColor[3];
    u8 tailColor[3];
    u32 depth;
    FieldG4Packet *packet;
    RenderTintMode *drawMode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    register GteMatrix *view asm("$15");
    int bias;
    u32 i;
    u32 last;
    FieldTrailPair *pair;
    s16 fade0;
    s16 fade1;

    view = D_800BCFA4.value;
    bias = (u16)D_800F3374;
    if (count >= 1000) {
        count -= 999;
        for (i = 0; i < count; i++) {
            history[i].head.vector.pad = 0;
            history[i].tail.vector.pad = 0;
        }
        return;
    }
    last = count - 1;
    pair = &history[last];
    for (i = 0; i < count; i++) {
        pair[1].head.words[0] = pair[0].head.words[0];
        pair[1].head.words[1] = pair[0].head.words[1];
        pair[1].tail.words[0] = pair[0].tail.words[0];
        pair[1].tail.words[1] = pair[0].tail.words[1];
        pair--;
    }
    if (head != 0) {
        history[0].head.vector.x = head->x;
        history[0].head.vector.y = head->y;
        history[0].head.vector.z = head->z;
        history[0].head.vector.pad = 1;
        history[0].tail.vector.x = tail->x;
        history[0].tail.vector.y = tail->y;
        history[0].tail.vector.z = tail->z;
        history[0].tail.vector.pad = 1;
    } else {
        history[0].head.vector.x = history[1].head.vector.x;
        history[0].head.vector.y = history[1].head.vector.y;
        history[0].head.vector.z = history[1].head.vector.z;
        history[0].tail.vector.x = history[1].tail.vector.x;
        history[0].tail.vector.y = history[1].tail.vector.y;
        history[0].tail.vector.z = history[1].tail.vector.z;
    }
    packet = (FieldG4Packet *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += count * sizeof(FieldG4Packet);
    if (color0 == 0) {
        headColor[0] = headColor[1] = headColor[2] = 0;
    } else {
        headColor[0] = color0[0] * scale / 128;
        headColor[1] = color0[1] * scale / 128;
        headColor[2] = color0[2] * scale / 128;
    }
    if (color1 == 0) {
        tailColor[0] = tailColor[1] = tailColor[2] = 0;
    } else {
        tailColor[0] = color1[0] * scale / 128;
        tailColor[1] = color1[1] * scale / 128;
        tailColor[2] = color1[2] * scale / 128;
    }
    {
        gte_ldrotmatrix((const GteMatrixWords *)view);
        gte_ldtransmatrix((const GteMatrixWords *)view);
    }
    for (i = 0; i < count; i++, history++, packet++) {
        u32 *depthOut = &depth;
        /* Non-volatile so GCC can hoist the opaque address out of the loop,
         * while retaining the indirect depth store and retail spill layout. */
        asm("" : "=r"(depthOut) : "0"(depthOut));
        if (history[0].head.vector.pad & history[0].tail.vector.pad & history[1].head.vector.pad &
            history[1].tail.vector.pad) {
            {
                GteShortVector *v1;
                GteShortVector *v2;
                v1 = &history[0].tail.vector;
                v2 = &history[1].head.vector;
                gte_lwc2_0_0(&history[0].head.vector);
                gte_lwc2_1_4(&history[0].head.vector);
                gte_lwc2_2_0(v1);
                gte_lwc2_3_4(v1);
                gte_lwc2_4_0(v2);
                gte_lwc2_5_4(v2);
            }
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_rtpt_command();
            gte_stmac0(depthOut);
            if (depth == 0) {
                return;
            }
            {
                register s16 *xy0 asm("$4") = &packet->x0;
                register s16 *xy1 asm("$3") = &packet->x1;
                s16 *xy2 = &packet->x2;
                gte_stsxy0_precise(xy0);
                gte_stsxy1_precise(xy1);
                gte_stsxy2_precise(xy2);
            }
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_avsz3_command();
            {
                register s32 z asm("$12");
                gte_getsz3(z);
                gte_cop2_hazard_slot();
                *depthOut = z >> 2;
            }
            depth -= bias;
            if (depth >= 0x1000) {
                return;
            }
            {
                GteShortVector *lastVertex = &history[1].tail.vector;
                gte_lwc2_0_0(lastVertex);
                gte_lwc2_1_4(lastVertex);
            }
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_rtps_command();
            SetPolyG4(packet);
            gte_stsxy2(&packet->x3);
            fade0 = 128 - i * 128 / count;
            fade1 = 128 - (i + 1) * 128 / count;
            packet->r0 = headColor[0] * fade0 / 128;
            packet->g0 = headColor[1] * fade0 / 128;
            packet->b0 = headColor[2] * fade0 / 128;
            packet->r1 = tailColor[0] * fade0 / 128;
            packet->g1 = tailColor[1] * fade0 / 128;
            packet->b1 = tailColor[2] * fade0 / 128;
            packet->r2 = headColor[0] * fade1 / 128;
            packet->g2 = headColor[1] * fade1 / 128;
            packet->b2 = headColor[2] * fade1 / 128;
            packet->r3 = tailColor[0] * fade1 / 128;
            packet->g3 = tailColor[1] * fade1 / 128;
            packet->b3 = tailColor[2] * fade1 / 128;
            TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], depth);
            if (mode != 0xFF) {
                drawMode = (RenderTintMode *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
                D_8009CDD8 += sizeof(RenderTintMode);
                SetDrawTPage((char *)drawMode, 0, 1, GetTPage(0, mode, 0, 0));
                if (packet) {
                    packet->code |= 2;
                    TILE_OT_ADDPRIM(ot, packet, link);
                }
                TILE_OT_ADDPRIM(ot, drawMode, link);
            } else if (packet) {
                TILE_OT_ADDPRIM(ot, packet, link);
            }
        }
    }
}

/* Full-screen 320x240 tile in `color` scaled by scale/128, linked at
 * `depth`; any blend mode other than 0xFF adds a semi-transparent draw mode. */
void func_800D1AE0(u8 *color, int scale, int abr, u32 depth)
{
    RenderTintTile *tile;
    RenderTintMode *mode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;

    tile = (RenderTintTile *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(RenderTintTile);
    SetTile(tile);
    tile->r = color[0] * scale / 128;
    tile->g = color[1] * scale / 128;
    tile->b = color[2] * scale / 128;
    if (depth < 0x1000) {
        tile->x = 0;
        tile->y = 0;
        tile->width = 320;
        tile->height = 240;
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], depth);
        if (abr != 0xFF) {
            mode = (RenderTintMode *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
            D_8009CDD8 += sizeof(RenderTintMode);
            SetDrawTPage((char *)mode, 0, 1, GetTPage(0, abr, 0, 0));
            if (tile) {
                tile->command |= 2;
                TILE_OT_ADDPRIM(ot, tile, link);
            }
            TILE_OT_ADDPRIM(ot, mode, link);
        } else if (tile) {
            TILE_OT_ADDPRIM(ot, tile, link);
        }
    }
}

int Geo_ClipAlternatingArcPoint(int radius, int divisor, int segment) {
    int angle = (segment << 10) / divisor;
    int direction = ((segment & 1) << 1) - 1;
    int product = rcos(angle) * radius;
    int y;
    GeomState *state;

    if (product < 0) {
        product += 0xFFF;
    }
    product >>= 12;
    y = product * direction;

    state = D_800B1624;
    state->clip_min_x = state->clip_min_y = -128;
    state = D_800B1624;
    state->clip_max_x = state->clip_max_y = 128;

    return Geo_ClipPoint(0, (s16)y, 0);
}

/* Matching debt: one SZ3 transfer pin and one empty depth-pointer constraint
 * per function. The shifts and stores are C; GTE transfers, commands and
 * hazard nops are separate macros. */

/* Projected one-pixel tile plus a 3x3 halo at a quarter of its colour. */
void func_800D1DEC(void *position, void *colorArg, int scale, int abr)
{
    u8 *color = colorArg;
    FieldTilePoint *point;
    RenderTintTile *glow;
    RenderTintMode *mode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    u32 depth;

    point = (FieldTilePoint *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(FieldTilePoint);
    glow = (RenderTintTile *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(RenderTintTile);
    gte_lwc2_0_0(position);
    gte_lwc2_1_4(position);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtps_command();
    SetTile1(point);
    SetSemiTrans(point, 1);
    SetTile(glow);
    glow->r = point->r = color[0] * scale / 128;
    glow->g = point->g = color[1] * scale / 128;
    glow->b = point->b = color[2] * scale / 128;
    glow->r >>= 2;
    glow->g >>= 2;
    glow->b >>= 2;
    gte_stsxy2(&point->x);
    {
        register s32 z asm("$12");
        s32 *out = (s32 *)&depth;
        asm volatile("" : "=r"(out) : "0"(out));
        gte_getsz3(z);
        gte_cop2_hazard_slot();
        z >>= 2;
        *out = z;
    }
    depth -= (u16)D_800F3374;
    if (depth < 0x1000) {
        glow->x = point->x - 1;
        glow->y = point->y - 1;
        glow->height = 3;
        glow->width = 3;
        AddPrim((u32 *)D_800B0E38.ordering[D_8009CDDC] + depth, (u32 *)point);
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], depth);
        if (abr != 0xFF) {
            mode = (RenderTintMode *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
            D_8009CDD8 += sizeof(RenderTintMode);
            SetDrawTPage((char *)mode, 0, 1, GetTPage(0, abr, 0, 0));
            if (glow) {
                glow->command |= 2;
                TILE_OT_ADDPRIM(ot, glow, link);
            }
            TILE_OT_ADDPRIM(ot, mode, link);
        } else if (glow) {
            TILE_OT_ADDPRIM(ot, glow, link);
        }
    }
}

/* One-pixel tile at the projected `position` in `color` scaled by
 * scale/128, linked at its view depth; any blend mode other than 0xFF adds a
 * semi-transparent draw mode. */
void func_800D2104(GteShortVector *position, u8 *color, int scale, int abr)
{
    FieldTilePoint *tile;
    RenderTintMode *mode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    u32 depth;

    tile = (FieldTilePoint *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(FieldTilePoint);
    gte_lwc2_0_0(position);
    gte_lwc2_1_4(position);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtps_command();
    SetTile1(tile);
    tile->r = color[0] * scale / 128;
    tile->g = color[1] * scale / 128;
    tile->b = color[2] * scale / 128;
    gte_stsxy2(&tile->x);
    {
        register s32 z asm("$12");
        s32 *out = (s32 *)&depth;
        asm volatile("" : "=r"(out) : "0"(out));
        gte_getsz3(z);
        gte_cop2_hazard_slot();
        z >>= 2;
        *out = z;
    }
    depth -= (u16)D_800F3374;
    if (depth < 0x1000) {
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], depth);
        if (abr != 0xFF) {
            mode = (RenderTintMode *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
            D_8009CDD8 += sizeof(RenderTintMode);
            SetDrawTPage((char *)mode, 0, 1, GetTPage(0, abr, 0, 0));
            if (tile) {
                tile->command |= 2;
                TILE_OT_ADDPRIM(ot, tile, link);
            }
            TILE_OT_ADDPRIM(ot, mode, link);
        } else if (tile) {
            TILE_OT_ADDPRIM(ot, tile, link);
        }
    }
}

/* Rectangle `length` deep and `breadth` wide in the local XZ plane at
 * `position`, turned by `rotation`, gouraud shaded from color0 (near edge)
 * to color1 (far edge) at intensity/128 and textured with the cell at
 * (u, v); mode 0xFF draws it opaque, anything else semi-transparent.
 * Matching debt: eight register pins and two empty constraints. The camera
 * and local matrix uploads are gte_ldrotmatrix and gte_ldtransmatrix. */
void func_800D2370(GteShortVector *position, GteRotation *rotation,
                   int length, int breadth, int u, int v, int texture_width,
                   int texture_height, int clut, RenderColor *color0,
                   RenderColor *color1, int intensity, int modeArg)
{
    register int mode asm("$21") = modeArg;
    GteShortVector corners[4];
    RenderColor black;
    GteMatrix matrix;
    u32 depth;
    FieldGt4Packet *packet;
    s16 scale;
    int bias;
    GteMatrix *view;

    view = D_800BCFA4.value;
    scale = intensity;
    bias = (u16)D_800F3374;
    black = D_800C22A0;
    packet = (FieldGt4Packet *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(FieldGt4Packet);
    SetPolyGT4(packet);
    if (mode == 0xFF) {
        packet->code &= ~2;
        packet->tpage = D_800F3370;
    } else {
        packet->code |= 2;
        packet->tpage = D_800F3370 | GetTPage(0, mode, 0, 0);
    }
    /* Keep rotation and length in registers across the packet setup calls. */
    asm("" : : "r"(rotation), "r"(length));
    packet->clut = clut;
    {
        register const GteMatrixWords *words asm("$19") = (const GteMatrixWords *)(view);
        gte_ldrotmatrix(words);
        gte_ldtransmatrix(words);
    }
    gte_lwc2_0_0(position);
    gte_lwc2_1_4(position);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    breadth /= 2;
    corners[0].x = corners[2].x = -breadth;
    corners[1].x = corners[3].x = breadth;
    corners[2].z = corners[3].z = length;
    corners[0].y = corners[1].y = corners[2].y = corners[3].y = 0;
    corners[0].z = corners[1].z = 0;
    {
        s32 *translation = matrix.t;
        gte_swc2_25_0(translation);
        gte_swc2_26_4(translation);
        gte_swc2_27_8(translation);
    }
    RotMatrixYXZ((GteShortVector *)rotation, &matrix);
    MulRotMatrix(&matrix);
    {
        register const GteMatrixWords *words asm("$16") = (const GteMatrixWords *)(&matrix);
        gte_ldrotmatrix(words);
        gte_ldtransmatrix(words);
    }
    {
        GteShortVector *v0;
        register GteShortVector *v1 asm("$4");
        GteShortVector *v2;
        v1 = &corners[1];
        v2 = &corners[2];
        v0 = &corners[0];
        gte_lwc2_0_0(v0);
        gte_lwc2_1_4(v0);
        gte_lwc2_2_0(v1);
        gte_lwc2_3_4(v1);
        gte_lwc2_4_0(v2);
        gte_lwc2_5_4(v2);
    }
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtpt_command();
    if (color0 == 0) {
        color0 = &black;
    }
    packet->r0 = packet->r1 = color0->r * scale / 128;
    packet->g0 = packet->g1 = color0->g * scale / 128;
    packet->b0 = packet->b1 = color0->b * scale / 128;
    if (color1 == 0) {
        color1 = &black;
    }
    packet->r2 = packet->r3 = color1->r * scale / 128;
    packet->g2 = packet->g3 = color1->g * scale / 128;
    packet->b2 = packet->b3 = color1->b * scale / 128;
    gte_stmac0(&depth);
    if (depth != 0) {
        {
            register s16 *xy0 asm("$4") = &packet->x0;
            register s16 *xy1 asm("$3") = &packet->x1;
            s16 *xy2 = &packet->x2;
            gte_stsxy0_precise(xy0);
            gte_stsxy1_precise(xy1);
            gte_stsxy2_precise(xy2);
        }
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_avsz3_command();
        {
            register s32 z asm("$12");
            register u32 *out asm("$5") = &depth;
            asm volatile("" : "=r"(out) : "0"(out));
            gte_getsz3(z);
            gte_cop2_hazard_slot();
            *out = z >> 2;
        }
        depth -= bias;
        if (depth < 0x1000) {
            {
                GteShortVector *last = &corners[3];
                gte_lwc2_0_0(last);
                gte_lwc2_1_4(last);
            }
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_rtps_command();
            packet->u0 = packet->u1 = u;
            packet->u2 = packet->u3 = u + texture_width - 1;
            packet->v0 = packet->v2 = v;
            packet->v1 = packet->v3 = v + texture_height - 1;
            gte_stsxy2(&packet->x3);
            AddPrim((unsigned int *)D_800B0E38.ordering[D_8009CDDC] + depth,
                    (unsigned int *)packet);
        }
    }
}
