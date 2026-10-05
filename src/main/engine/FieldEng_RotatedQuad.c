#include "common.h"
#include "pe1/gte.h"
#include "pe1/field_rotated_quad.h"

/* Rectangle `length` deep and `breadth` wide in the local XZ plane at
 * `position`, turned by `rotation`, gouraud shaded from color0 (near edge)
 * to color1 (far edge) at intensity/128 and textured with the cell at
 * (u, v); mode 0xFF draws it opaque, anything else semi-transparent.
 * Matching debt: fourteen register pins and four empty constraints. CPU
 * matrix loads and depth arithmetic are C; each GTE instruction is separate. */
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
