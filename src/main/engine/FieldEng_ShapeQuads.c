#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_prim.h"
#include "pe1/field_shape_quads.h"

/* Matched field-shape quads. CPU-side matrix loads, depth scaling and packet
 * writes are C; each GTE instruction and hazard nop has its own macro.
 * Matching debt: pinned transfer registers/pointers and empty constraints,
 * including the template-copy endpoint used to preserve preheader order. */
void func_800CEE20(GteShortVector *position, GteRotation *rotation,
                   int scale_x, int scale_y, int texture, int clut,
                   int page, int intensity, RenderColor *color)
{
    FieldStripPacket template;
    GteVector scale;
    GteRotation level;
    GteMatrix matrix;
    s32 depth;
    register s32 *depthOut asm("$11");
    FieldStripPacket *packet;
    register s32 *view asm("$8");
    GteShortVector *vertex;
    int r;
    register int g asm("$3");
    int b;
    int u, v;
    int width, height;
    int bias;
    int i;

    level = D_800C2268;
    if (rotation == 0) {
        rotation = &level;
    }
    template.tag.length = 9;
    template.code = 0x2C;
    if (page == 0xFF) {
        template.code &= ~2;
        template.tpage = D_800F3368.tpage;
    } else {
        template.code |= 2;
        width = GetTPage(0, page, 0, 0);
        template.tpage = D_800F3368.tpage | width;
    }
    view = D_800BCFA4.value;
    template.clut = clut;
    if (color == 0) {
        g = intensity;
        b = g;
        r = g;
    } else {
        r = intensity * color->r / 128;
        g = intensity * color->g / 128;
        b = intensity * color->b / 128;
    }
    asm("" : : : "$5");
    template.r0 = r;
    template.g0 = g;
    template.b0 = b;
    if (D_800F3368.parameter06 != 0) {
        u = texture & 0xF;
        v = 0;
        if (u >= 8) {
            u -= 8;
            v = 0x20;
        }
        u <<= 4;
        v += texture / 16 * 16;
    } else {
        u = (texture & 0xF) << 4;
        v = texture / 16 * 16;
    }
    if (D_800F3368.palette == 4 && D_800F3428 != 0) {
        v += 0x60;
    }
    width = D_800F3368.extent_x;
    height = D_800F3368.extent_y;
    asm("" : : "r"(scale_x), "r"(scale_y));
    scale.x = scale_x * (width >> 4);
    template.u0 = template.u2 = u;
    template.v1 = template.v0 = v;
    scale.y = scale_y * (height >> 4);
    template.v2 = template.v3 = v + height - 1;
    scale.z = 0x1000;
    template.u1 = template.u3 = u + width - 1;
    {
        const u32 *words = (const u32 *)(view);
        register u32 a asm("$12");
        register u32 b asm("$13");
        register u32 c asm("$14");
        asm volatile("" : "=r"(words) : "0"(words) : "memory");
        a = words[0];
        b = words[1];
        gte_ctc2_0(a);
        gte_ctc2_1(b);
        a = words[2];
        b = words[3];
        c = words[4];
        gte_ctc2_2(a);
        gte_ctc2_3(b);
        gte_ctc2_4(c);
        a = words[5];
        b = words[6];
        gte_ctc2_5(a);
        c = words[7];
        gte_ctc2_6(b);
        gte_ctc2_7(c);
    }
    gte_lwc2_0_0(position);
    gte_lwc2_1_4(position);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    packet = (FieldStripPacket *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += D_800E1210[D_800F3368.parameter0A] * sizeof(FieldStripPacket);
    bias = (u16)D_800F3368.depth;
    RotMatrixYXZ(rotation, &matrix);
    Gte_ScaleMatrix(&matrix, &scale);
    gte_swc2_25_0(matrix.t);
    gte_swc2_26_4(matrix.t);
    gte_swc2_27_8(matrix.t);
    if (rotation->flags == 1) {
        MulRotMatrix(&matrix);
    }
    {
        register const u32 *words asm("$17") = (const u32 *)(&matrix);
        register u32 a asm("$12");
        register u32 b asm("$13");
        register u32 c asm("$14");
        asm volatile("" : "=r"(words) : "0"(words) : "memory");
        a = words[0];
        b = words[1];
        gte_ctc2_0(a);
        gte_ctc2_1(b);
        a = words[2];
        b = words[3];
        c = words[4];
        gte_ctc2_2(a);
        gte_ctc2_3(b);
        gte_ctc2_4(c);
        a = words[5];
        b = words[6];
        gte_ctc2_5(a);
        c = words[7];
        gte_ctc2_6(b);
        gte_ctc2_7(c);
    }
    vertex = D_800E13BC[D_800F3368.parameter0A];
    i = 0;
    if (i < D_800E1210[D_800F3368.parameter0A]) {
        {
            register void *copyEnd asm("$14") = &template.x3;
            asm("" : : "r"(copyEnd));
        }
        depthOut = &depth;
        for (;;) {
            {
                register GteShortVector *v1 = &vertex[1];
                register GteShortVector *v2 asm("$2") = &vertex[2];
                asm volatile("" : "=r"(v1), "=r"(v2) : "0"(v1), "1"(v2));
                gte_lwc2_0_0(vertex);
                gte_lwc2_1_4(vertex);
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
                register s16 *p0 asm("$4") = &packet->x0;
                register s16 *p1 = &packet->x1;
                register s16 *p2 asm("$2") = &packet->x2;
                asm volatile("" : "=r"(p0), "=r"(p1), "=r"(p2) : "0"(p0), "1"(p1), "2"(p2));
                gte_stsxy0_precise(p0);
                gte_stsxy1_precise(p1);
                gte_stsxy2_precise(p2);
            }
            asm volatile("" : "=r"(depthOut) : "0"(depthOut));
            gte_stmac0(depthOut);
            if (depth == 0) {
                break;
            }
            {
                register s32 z asm("$12");
                gte_getsz3(z);
                gte_cop2_hazard_slot();
                z >>= 2;
                *depthOut = z;
                asm("" : : "m"(*depthOut) : "$2", "memory");
            }
            {
                register GteShortVector *v3 = &vertex[3];
                asm volatile("" : "=r"(v3) : "0"(v3));
                gte_lwc2_0_0(v3);
                gte_lwc2_1_4(v3);
            }
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_rtps_command();
            depth -= bias;
            if ((u32)depth < 0x1000) {
                FieldStripLink link;

                {
                    register s16 *p3 = &packet->x3;
                    asm volatile("" : "=r"(p3) : "0"(p3));
                    gte_stsxy2(p3);
                }
                packet->tag.address = STRIP_OT(depth)->address;
                link.tag = &packet->tag;
                STRIP_OT(depth)->address = link.word;
            }
            i++;
            packet++;
            vertex += 4;
            if (i >= D_800E1210[D_800F3368.parameter0A])
                break;
        }
    }
}
