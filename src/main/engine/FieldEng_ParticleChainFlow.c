#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_prim.h"
#include "pe1/render_object.h"
#include "pe1/field_particle_chain.h"
#include "pe1/field_textured_strip.h"

void func_800C5538(FieldChainRecord *record)
{
    GteShortVector tailIn;
    GteShortVector tail;
    GteShortVector headIn;
    GteShortVector head;
    GteMatrix matrix;
    GteShortVector angles;
    GteMatrix rotation;
    GteVector previous;
    GteVector toPlayer;
    GteVector cross;
    FieldChainLink *link;
    unsigned int i;
    int run;
    int turn;

    link = record->links;
    memset(&tail, 0, sizeof(tail));
    tail.x = -record->length;
    tail.y = 0;
    tail.z = 0;
    tailIn = tail;
    memset(&head, 0, sizeof(head));
    head.x = record->length;
    head.y = 0;
    head.z = 0;
    headIn = head;
    matrix = record->matrix;
    record->field0E = 0;
    for (i = 0; i < record->count; i++, link++) {
        ApplyMatrixSV(&matrix, &tailIn, &tail);
        ApplyMatrixSV(&matrix, &headIn, &head);
        link->edgeA.x = head.x + matrix.t[0];
        link->edgeA.y = head.y + matrix.t[1];
        link->edgeA.z = head.z + matrix.t[2];
        link->edgeB.x = tail.x + matrix.t[0];
        link->edgeB.y = tail.y + matrix.t[1];
        link->edgeB.z = tail.z + matrix.t[2];
        if (i >= 2)
            record->bend = 1;
        else
            record->bend = 0;
        if (record->bend == 1) {
            previous.x = link[-1].matrix.t[0] - link[-2].matrix.t[0];
            previous.y = 0;
            previous.z = link[-1].matrix.t[2] - link[-2].matrix.t[2];
            toPlayer.x = D_8009D254->x.part.integer - link[-2].matrix.t[0];
            toPlayer.y = 0;
            toPlayer.z = D_8009D254->z.part.integer - link[-2].matrix.t[2];
            OuterProduct0(&previous, &toPlayer, &cross);
            run = Gte_ISqrt(previous.x * previous.x + previous.z * previous.z);
            Gte_Atan2(run, Gte_ISqrt(toPlayer.x * toPlayer.x + toPlayer.z * toPlayer.z));
            if (cross.y > 0)
                turn = rand() % 512 + 0x40;
            else
                turn = -(rand() % 512 + 0x40);
            angles.y = turn;
            angles.x = 0;
            angles.z = 0;
        } else {
            angles.x = link->tiltX;
            angles.y = link->tiltY;
            angles.z = 0;
        }
        if (i >= 6) {
            angles.x = 0;
            angles.y = 0;
            angles.z = 0;
        }
        RotMatrix(&angles, &rotation);
        rotation.t[0] = 0;
        rotation.t[1] = 0;
        rotation.t[2] = -record->depth;
        gte_CompMatrix(&matrix, &rotation, &matrix);
        link->matrix = matrix;
    }
}

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
