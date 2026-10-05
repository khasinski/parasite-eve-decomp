#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_prim.h"
#include "pe1/render_object.h"
#include "pe1/field_textured_strip.h"

/* Project and link textured quads between successive strip edges.
 * Matching debt: five pins and three empty barriers preserve matrix loads
 * and packet-header constant order. CPU loads are C; COP2 transfers each
 * use their own instruction macro. */
void func_800C5A40(FieldTexturedStrip *strip)
{
    FieldStripNode *node;
    FieldStripPacket *packet;
    GteShortVector quad[4];
    s32 p;
    s32 flag;
    u32 i;
    u8 cell;
    int step;
    u32 depth;
    FieldStripLink link;

    node = strip->nodes;
    i = 0;
    D_800F33B4 = FIELD_ENGINE_SCRATCH;
    packet = (FieldStripPacket *)(D_800B0E58[D_8009CDDC] + D_8009CDD8);
    D_800F33B4->clutX = strip->clut << 4;
    D_800F33B4->clutY = strip->clut >> 4;
    while (i < strip->count - 1) {
        cell = strip->cell;
        step = node->cellStep;
        D_800F33B4->v = cell & 0xF0;
        D_800F33B4->u = (strip->cell - D_800F33B4->v) << 4;
        cell += step;
        D_800F33B4->v = cell & 0xF0;
        D_800F33B4->u = (cell - D_800F33B4->v) << 4;
        quad[0] = node->edgeA;
        quad[1] = node->edgeB;
        quad[2] = node[1].edgeA;
        quad[3] = node[1].edgeB;
        {
            s32 **slot;
            register const GteMatrixWords *matrix asm("$8");
            register u32 a asm("$12");
            register u32 b asm("$13");
            register u32 c asm("$14");
            asm volatile("" : : : "memory");
            slot = &D_800BCFA4.value;
            asm volatile("" : "=r"(slot) : "0"(slot));
            matrix = (const GteMatrixWords *)*slot;
            a = matrix->r11_r12;
            b = matrix->r13_r21;
            gte_ctc2_0(a);
            gte_ctc2_1(b);
            a = matrix->r22_r23;
            b = matrix->r31_r32;
            c = matrix->r33_pad;
            gte_ctc2_2(a);
            gte_ctc2_3(b);
            gte_ctc2_4(c);
            a = matrix->tx;
            b = matrix->ty;
            gte_ctc2_5(a);
            c = matrix->tz;
            gte_ctc2_6(b);
            gte_ctc2_7(c);
        }
        depth = RotTransPers4(&quad[0], &quad[1], &quad[2], &quad[3],
                              &packet->x0, &packet->x1, &packet->x2,
                              &packet->x3, &p, &flag);
        func_800C608C(node->brightness, node->rgb, &packet->r0);
        packet->u0 = D_800F33B4->u;
        packet->v0 = D_800F33B4->v;
        packet->u1 = D_800F33B4->u + D_800F345C;
        packet->v1 = D_800F33B4->v;
        packet->u2 = D_800F33B4->u;
        packet->v2 = D_800F33B4->v + D_800F345D;
        packet->u3 = D_800F33B4->u + D_800F345C;
        packet->v3 = D_800F33B4->v + D_800F345D;
        packet->tpage = D_800E27AC;
        packet->clut = GetClut(D_800F341C + D_800F33B4->clutX,
                               D_800F341E + D_800F33B4->clutY);
        {
            register u8 code asm("$8");
            u8 length = 9;
            asm("" : : "r"(length) : "$8");
            code = 0x2C;
            packet->tag.length = length;
            packet->code = code;
        }
        /* PSY-Q setSemiTrans(packet, D_800F337A). */
        if (D_800F337A) {
            packet->code = packet->code | 2;
        } else {
            packet->code = packet->code & ~2;
        }
        if (depth != 0 && depth < 0x1000 && node->visible) {
            packet->tag.address = STRIP_OT(depth)->address;
            link.tag = &packet->tag;
            STRIP_OT(depth)->address = link.word;
        }
        i++;
        node++;
        packet++;
        D_8009CDD8 += sizeof(FieldStripPacket);
    }
}
