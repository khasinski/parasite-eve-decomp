/* WIP: score 6103, NOT a retail byte-match. See FieldEng_TexturedRibbon.md. */
/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/field_textured_ribbon.h"

/* Draw a textured ribbon `width` wide through `count` points: every joint
 * is widened across the screen direction of the segment that leaves it,
 * shaded from color0 to color1 (each at scale/128) along the ribbon and
 * textured from u..u+du by v..v+height. Mode 0xFF draws opaque, anything
 * else semi-transparent. */
void func_800D3114(GteShortVector *points, s16 count, int width, int u, int v,
                   int du, int height, int tpage, int clut, int scale,
                   RenderColor *color0, RenderColor *color1, int mode)
{
    GteShortVector *point;
    GteShortVector angles;
    GteShortVector right;
    GteShortVector left;
    FieldRibbonColor color0Scaled;
    FieldRibbonColor color1Scaled;
    FieldRibbonColor color;
    FieldScreenPoint screen0;
    FieldScreenPoint screen1;
    GteMatrix matrix;
    register GteMatrix *localMatrix asm("$19");
    u32 unused;
    u32 depth;
    FieldRibbonPacket *packet;
    int i;
    u32 previousDepth;
    int bias;
    int t;
    u32 shade;
    int dx;
    int dy;
    int angle;
    int joint;
    register GteShortVector *rightPtr asm("$22");
    register int loopCount asm("$23");

    packet = (FieldRibbonPacket *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += count * sizeof(FieldRibbonPacket);
    bias = (u16)D_800F3374;
    color0Scaled.bytes.rgb[0] = color0->r * scale / 128;
    color0Scaled.bytes.rgb[1] = color0->g * scale / 128;
    color0Scaled.bytes.rgb[2] = color0->b * scale / 128;
    color1Scaled.bytes.rgb[0] = color1->r * scale / 128;
    color1Scaled.bytes.rgb[1] = color1->g * scale / 128;
    color1Scaled.bytes.rgb[2] = color1->b * scale / 128;
    right.x = width / 2;
    previousDepth = 0;
    right.y = right.z = 0;
    left.y = left.z = 0;
    left.x = -width / 2;
    angles.x = angles.y = 0;
    localMatrix = &matrix;
    rightPtr = &right;
    point = points;
    loopCount = count;
    for (i = 0; i < loopCount; i++) {
        {
            const GteMatrixWords *words = (const GteMatrixWords *)(D_800BCFA4.value);
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
        {
            const void *v1 = &point[1];
            asm volatile("" : "=r"(v1) : "0"(v1));
            gte_lwc2_0_0(&point[0]);
            gte_lwc2_1_4(&point[0]);
            gte_lwc2_2_0(v1);
            gte_lwc2_3_4(v1);
            gte_lwc2_4_0(&point[0]);
            gte_lwc2_5_4(&point[0]);
        }
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_rtpt_command();
        packet->tag.length = 12;
        packet->c0.bytes.code = 0x3C;
        if (mode == 0xFF) {
            packet->c0.bytes.code = 0x3C;
            packet->tpage = tpage;
        } else {
            packet->c0.bytes.code = 0x3E;
            packet->tpage = GetTPage(0, mode, 0, 0) | tpage;
        }
        packet->clut = clut;
        {
            void *xy0 = &screen0;
            void *xy1 = &screen1;
            void *xy2 = &unused;
            asm volatile("" : : "r"(xy0), "r"(xy1), "r"(xy2));
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
            asm volatile("" : : : "memory");
        }
        depth -= bias;
        if (depth >= 0x1000) {
            return;
        }
        dx = screen1.x - screen0.x;
        dy = screen1.y - screen0.y;
        angle = Gte_Atan2(dy, dx) + 0x400;
        t = (i << 12) / loopCount;
        LoadAverageCol(&color0Scaled, &color1Scaled, 0x1000 - t, t, &color);
        if (i == 0) {
            joint = angle;
            angles.z = joint;
            RotMatrix(&angles, localMatrix);
            gte_lwc2_0_0(&point[0]);
            gte_lwc2_1_4(&point[0]);
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_mvmva_rotation_v0_translation_sf12();
            packet->c0.bytes.rgb[0] = packet->c1.bytes.rgb[0] = color.bytes.rgb[0];
            packet->c0.bytes.rgb[1] = packet->c1.bytes.rgb[1] = color.bytes.rgb[1];
            packet->c0.bytes.rgb[2] = packet->c1.bytes.rgb[2] = color.bytes.rgb[2];
            gte_swc2_25_0(matrix.t);
            gte_swc2_26_4(matrix.t);
            gte_swc2_27_8(matrix.t);
            {
                const GteMatrixWords *words = (const GteMatrixWords *)(localMatrix);
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
            {
                    const void *v1 = &left;
                    asm volatile("" : "=r"(v1) : "0"(v1));
                gte_lwc2_0_0(rightPtr);
                gte_lwc2_1_4(rightPtr);
                gte_lwc2_2_0(v1);
                gte_lwc2_3_4(v1);
                gte_lwc2_4_0(rightPtr);
                gte_lwc2_5_4(rightPtr);
            }
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_rtpt_command();
            packet->u0 = packet->u1 = u;
            packet->v0 = v;
            packet->v1 = v + height;
            previousDepth = depth;
            {
                void *xy0 = &packet->p0;
                void *xy1 = &packet->p1;
                void *xy2 = &unused;
                asm volatile("" : : "r"(xy0), "r"(xy1), "r"(xy2));
                gte_stsxy0_precise(xy0);
                gte_stsxy1_precise(xy1);
                gte_stsxy2_precise(xy2);
            }
            packet++;
        } else if (i < loopCount - 1) {
            joint = angle;
            angles.z = joint;
            RotMatrix(&angles, localMatrix);
            gte_lwc2_0_0(&point[0]);
            gte_lwc2_1_4(&point[0]);
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_mvmva_rotation_v0_translation_sf12();
            shade = color.word;
            packet[-1].c3.word = shade;
            packet[-1].c2.word = shade;
            gte_swc2_25_0(matrix.t);
            gte_swc2_26_4(matrix.t);
            gte_swc2_27_8(matrix.t);
            {
                const GteMatrixWords *words = (const GteMatrixWords *)(localMatrix);
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
            {
                    const void *v1 = &left;
                    asm volatile("" : "=r"(v1) : "0"(v1));
                gte_lwc2_0_0(rightPtr);
                gte_lwc2_1_4(rightPtr);
                gte_lwc2_2_0(v1);
                gte_lwc2_3_4(v1);
                gte_lwc2_4_0(rightPtr);
                gte_lwc2_5_4(rightPtr);
            }
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_rtpt_command();
            packet[-1].v2 = v;
            packet[-1].v3 = v + height;
            packet[-1].u2 = packet[-1].u3 = u + du * i / loopCount;
            {
                void *xy0 = &packet[-1].p2;
                void *xy1 = &packet[-1].p3;
                void *xy2 = &unused;
                asm volatile("" : : "r"(xy0), "r"(xy1), "r"(xy2));
                gte_stsxy0_precise(xy0);
                gte_stsxy1_precise(xy1);
                gte_stsxy2_precise(xy2);
            }
            AddPrim((unsigned int *)D_800B0E38.ordering[D_8009CDDC] + previousDepth,
                    (unsigned int *)&packet[-1]);
            packet->p0.word = packet[-1].p2.word;
            packet->p1.word = packet[-1].p3.word;
            packet->c0.bytes.rgb[0] = packet->c1.bytes.rgb[0] = color.bytes.rgb[0];
            packet->c0.bytes.rgb[1] = packet->c1.bytes.rgb[1] = color.bytes.rgb[1];
            packet->c0.bytes.rgb[2] = packet->c1.bytes.rgb[2] = color.bytes.rgb[2];
            packet->u0 = packet->u1 = packet[-1].u2;
            packet->v0 = v;
            packet->v1 = v + height;
            previousDepth = depth;
            packet++;
        } else {
            joint = angle;
            angles.z = joint;
            RotMatrix(&angles, localMatrix);
            gte_lwc2_0_0(&point[0]);
            gte_lwc2_1_4(&point[0]);
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_mvmva_rotation_v0_translation_sf12();
            shade = color.word;
            packet[-1].c3.word = shade;
            packet[-1].c2.word = shade;
            gte_swc2_25_0(matrix.t);
            gte_swc2_26_4(matrix.t);
            gte_swc2_27_8(matrix.t);
            {
                const GteMatrixWords *words = (const GteMatrixWords *)(localMatrix);
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
            {
                    const void *v1 = &left;
                    asm volatile("" : "=r"(v1) : "0"(v1));
                gte_lwc2_0_0(rightPtr);
                gte_lwc2_1_4(rightPtr);
                gte_lwc2_2_0(v1);
                gte_lwc2_3_4(v1);
                gte_lwc2_4_0(rightPtr);
                gte_lwc2_5_4(rightPtr);
            }
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_rtpt_command();
            packet[-1].v2 = v;
            packet[-1].v3 = v + height;
            packet[-1].u2 = packet[-1].u3 = u + du * i / loopCount;
            {
                void *xy0 = &packet[-1].p2;
                void *xy1 = &packet[-1].p3;
                void *xy2 = &unused;
                asm volatile("" : : "r"(xy0), "r"(xy1), "r"(xy2));
                gte_stsxy0_precise(xy0);
                gte_stsxy1_precise(xy1);
                gte_stsxy2_precise(xy2);
            }
            AddPrim((unsigned int *)D_800B0E38.ordering[D_8009CDDC] + previousDepth,
                    (unsigned int *)&packet[-1]);
            packet->p0.word = packet[-1].p2.word;
            packet->p1.word = packet[-1].p3.word;
            packet->c0.bytes.rgb[0] = packet->c1.bytes.rgb[0] = color.bytes.rgb[0];
            packet->c0.bytes.rgb[1] = packet->c1.bytes.rgb[1] = color.bytes.rgb[1];
            packet->c0.bytes.rgb[2] = packet->c1.bytes.rgb[2] = color.bytes.rgb[2];
            packet->u0 = packet->u1 = packet[-1].u2;
            packet->v0 = v;
            packet->v1 = v + height;
            angles.z = joint;
            RotMatrix(&angles, localMatrix);
            {
                const GteMatrixWords *words = (const GteMatrixWords *)(D_800BCFA4.value);
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
            gte_lwc2_0_0(&point[1]);
            gte_lwc2_1_4(&point[1]);
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_mvmva_rotation_v0_translation_sf12();
            packet->c2.word = packet->c3.word = color1Scaled.word;
            gte_swc2_25_0(matrix.t);
            gte_swc2_26_4(matrix.t);
            gte_swc2_27_8(matrix.t);
            {
                const GteMatrixWords *words = (const GteMatrixWords *)(localMatrix);
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
            {
                    const void *v1 = &left;
                    asm volatile("" : "=r"(v1) : "0"(v1));
                gte_lwc2_0_0(rightPtr);
                gte_lwc2_1_4(rightPtr);
                gte_lwc2_2_0(v1);
                gte_lwc2_3_4(v1);
                gte_lwc2_4_0(rightPtr);
                gte_lwc2_5_4(rightPtr);
            }
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_rtpt_command();
            packet->v2 = v;
            packet->u2 = packet->u3 = u + du;
            packet->v3 = v + height;
            {
                void *xy0 = &packet->p2;
                void *xy1 = &packet->p3;
                void *xy2 = &unused;
                asm volatile("" : : "r"(xy0), "r"(xy1), "r"(xy2));
                gte_stsxy0_precise(xy0);
                gte_stsxy1_precise(xy1);
                gte_stsxy2_precise(xy2);
            }
            AddPrim((unsigned int *)D_800B0E38.ordering[D_8009CDDC] + depth,
                    (unsigned int *)packet);
        }
        point++;
    }
}
