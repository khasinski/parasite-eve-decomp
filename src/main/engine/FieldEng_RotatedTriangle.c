#include "common.h"
#include "pe1/gte.h"
#include "pe1/field_rotated_triangle.h"

/* Triangle with its apex at `position` and a `breadth` wide base `length`
 * away along local Z, turned by `rotation` and shaded per corner from
 * color0..color2 at intensity/128; mode 0xFF draws it opaque, anything
 * else adds a semi-transparent draw mode.
 * Matching debt: twelve register pins and four empty constraints. CPU
 * loads and depth arithmetic are C; every GTE instruction is separate. */
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
}
