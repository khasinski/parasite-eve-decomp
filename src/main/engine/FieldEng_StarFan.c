/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/field_star_fan.h"

/* `segments` triangles around `position`: the rim points alternate between
 * radius `width` and `height`, color0 shades the centre and color1 the
 * rim at intensity/128; a nonzero rotation->flags also applies the view
 * rotation. Drawing stops at the first triangle beyond the depth range.
 * Matching debt: twelve register pins and five empty constraints. CPU
 * matrix loads and depth arithmetic are C; each GTE instruction is separate. */
void func_800D004C(GteShortVector *position, int width, int height, int segments,
                   GteRotation *rotation, int scale_x, int scale_y,
                   RenderColor *color0, RenderColor *color1, int intensity,
                   int mode)
{
    FieldG3Packet template;
    s16 radii[2];
    GteShortVector vertices[3];
    GteShortVector anchor;
    GteShortVector defaultRotation = D_800C2260;
    GteVector scale = D_800C2290;
    RenderColor centre;
    RenderColor rim;
    GteMatrix matrix;
    u32 depth;
    GteMatrix *view;
    FieldG3Packet *packet;
    RenderTintMode *drawMode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    int bias;
    int i;
    int angle;
    int next;
    int lastY;

    if (segments < 4)
        return;
    bias = (u16)D_800F3374;
    view = (GteMatrix *)D_800BCFA4.value;
    /* Preserve position/color allocation without pinning the arguments. */
    asm("" : : "r"(position));
    if (color0 == 0) {
        centre.r = centre.g = centre.b = 0;
    } else {
        centre.r = color0->r * intensity / 128;
        centre.g = color0->g * intensity / 128;
        centre.b = color0->b * intensity / 128;
    }
    if (color1 == 0) {
        rim.r = rim.g = rim.b = 0;
    } else {
        rim.r = color1->r * intensity / 128;
        rim.g = color1->g * intensity / 128;
        rim.b = color1->b * intensity / 128;
    }
    {
        const GteMatrixWords *words = (const GteMatrixWords *)(view);
        register u32 a asm("$12");
        register u32 b asm("$13");
        register u32 c asm("$14");
        a = words->r11_r12;
        b = words->r13_r21;
        gte_ctc2_0(a);
        gte_ctc2_1(b);
        a = words->r22_r23;
        b = words->r31_r32;
        c = words->r33_pad;
        gte_ctc2_2(a);
        gte_ctc2_3(b);
        gte_ctc2_4(c);
        a = words->tx;
        b = words->ty;
        gte_ctc2_5(a);
        c = words->tz;
        gte_ctc2_6(b);
        gte_ctc2_7(c);
    }
    anchor.x = position->x;
    anchor.y = position->y;
    anchor.z = position->z;
    gte_lwc2_0_0(&anchor);
    gte_lwc2_1_4(&anchor);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    if (rotation == 0)
        rotation = (GteRotation *)&defaultRotation;
    scale.x = scale_x;
    scale.y = scale_y;
    {
        s32 *translation = matrix.t;
        gte_swc2_25_0(translation);
        gte_swc2_26_4(translation);
        gte_swc2_27_8(translation);
    }
    RotMatrixYXZ((GteShortVector *)rotation, &matrix);
    Gte_ScaleMatrix(&matrix, &scale);
    if (rotation->flags)
        MulRotMatrix(&matrix);
    {
        register const GteMatrixWords *words asm("$16") = (const GteMatrixWords *)(&matrix);
        register u32 a asm("$12");
        register u32 b asm("$13");
        register u32 c asm("$14");
        asm volatile("" : "=r"(words) : "0"(words));
        a = words->r11_r12;
        b = words->r13_r21;
        gte_ctc2_0(a);
        gte_ctc2_1(b);
        a = words->r22_r23;
        b = words->r31_r32;
        c = words->r33_pad;
        gte_ctc2_2(a);
        gte_ctc2_3(b);
        gte_ctc2_4(c);
        a = words->tx;
        b = words->ty;
        gte_ctc2_5(a);
        c = words->tz;
        gte_ctc2_6(b);
        gte_ctc2_7(c);
    }
    SetPolyG3(&template);
    template.r0 = centre.r;
    template.g0 = centre.g;
    template.b0 = centre.b;
    template.r2 = template.r1 = rim.r;
    template.g2 = template.g1 = rim.g;
    template.b2 = template.b1 = rim.b;
    packet = (FieldG3Packet *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += segments * sizeof(FieldG3Packet);
    vertices[0].x = vertices[0].y = vertices[0].z = vertices[1].z = vertices[2].z = 0;
    radii[0] = width;
    radii[1] = height;
    for (i = 0; i < segments; i++, packet++) {
        /* Prevent a separate induction pointer for packet->code. */
        asm volatile("" : "=r"(packet) : "0"(packet));
        angle = (i << 12) / segments;
        next = ((i + 1) << 12) / segments;
        vertices[1].x = rcos(angle) * radii[i & 1] / 4096;
        vertices[1].y = rsin(angle) * radii[i & 1] / 4096;
        vertices[2].x = rcos(next) * radii[(i + 1) & 1] / 4096;
        lastY = rsin(next) * radii[(i + 1) & 1] / 4096;
        /* Keep vertex-address setup after the final coordinate calculation. */
        asm volatile("" : : : "$4");
        {
            register GteShortVector *v0 asm("$4");
            register GteShortVector *v1 asm("$3");
            GteShortVector *v2;
            v0 = &vertices[0];
            v1 = &vertices[1];
            vertices[2].y = lastY;
            v2 = &vertices[2];
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
        *packet = template;
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
            u32 *out = &depth;
            asm volatile("" : "=r"(out) : "0"(out));
            gte_getsz3(z);
            gte_cop2_hazard_slot();
            *out = z >> 2;
        }
        depth -= bias;
        if (depth >= 0x1000)
            return;
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], depth);
        if (mode != 0xFF) {
            drawMode = (RenderTintMode *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
            D_8009CDD8 += sizeof(RenderTintMode);
            SetDrawMode((char *)drawMode, 0, 1, GetTPage(0, mode, 0, 0));
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
