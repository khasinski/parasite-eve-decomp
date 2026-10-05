#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_prim.h"
#include "pe1/render_object.h"
#include "pe1/field_billboard.h"
/* MASPSX_FLAGS: --expand-div */

/* Matching debt: four register pins and one empty slot-address barrier.
 * Matrix loads are C; each GTE transfer uses its individual macro. */
void func_800C3B04(FieldBillboard *board)
{
    FieldStripPacket *packet;
    int width;
    int height;
    int halfWidth;
    int halfHeight;
    int distance;
    int projectedWidth;
    int projectedHeight;
    FieldStripLink link;

    D_800E284C = FIELD_BILLBOARD_SCRATCH;
    D_800E284C->v = board->cell & 0xF0;
    D_800E284C->u = (board->cell - D_800E284C->v) << 4;
    D_800E284C->clutX = board->clut << 4;
    D_800E284C->clutY = board->clut >> 4;
    width = (D_800F345C * board->scaleX) >> 12;
    height = (D_800F345D * board->scaleY) >> 12;
    if (board->brightness != 0x80) {
        D_800E284C->channel = board->rgb[0];
        D_800E284C->channel *= board->brightness;
        if (D_800E284C->channel > 0x7FFF) {
            D_800E284C->channel = 0x7FFF;
        }
        D_800E284C->channel >>= 7;
        D_800E284C->rgb[0] = D_800E284C->channel;
        D_800E284C->channel = board->rgb[1];
        D_800E284C->channel *= board->brightness;
        if (D_800E284C->channel > 0x7FFF) {
            D_800E284C->channel = 0x7FFF;
        }
        D_800E284C->channel >>= 7;
        D_800E284C->rgb[1] = D_800E284C->channel;
        D_800E284C->channel = board->rgb[2];
        D_800E284C->channel *= board->brightness;
        if (D_800E284C->channel > 0x7FFF) {
            D_800E284C->channel = 0x7FFF;
        }
        D_800E284C->channel >>= 7;
        D_800E284C->rgb[2] = D_800E284C->channel;
    } else {
        D_800E284C->rgb[0] = board->rgb[0];
        D_800E284C->rgb[1] = board->rgb[1];
        D_800E284C->rgb[2] = board->rgb[2];
    }
    {
        s32 **slot;
        register const GteMatrixWords *matrix asm("$9");
        register u32 a asm("$12");
        register u32 b asm("$13");
        register u32 c asm("$14");
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
    D_800E284C->depth = RotTransPers(&board->position, &D_800E284C->screen.word,
                                     &D_800E284C->p, &D_800E284C->flag);
    D_800E284C->centerX = D_800E284C->screen.word;
    D_800E284C->centerY = D_800E284C->screen.xy.y;
    distance = D_800BCFA8.distance[0];
    projectedWidth = (width << 5) * distance / (D_800E284C->depth * 4 + distance);
    projectedHeight =
        (height << 5) * distance / (D_800E284C->depth * 4 + distance);
    halfWidth = projectedWidth >> 1;
    halfHeight = projectedHeight >> 1;
    packet = (FieldStripPacket *)(D_800B0E58[D_8009CDDC] + D_8009CDD8);
    packet->tag.length = 9;
    packet->code = 0x2C;
    /* PSY-Q setSemiTrans(packet, D_800F337A). */
    if (D_800F337A) {
        packet->code = packet->code | 2;
    } else {
        packet->code = packet->code & ~2;
    }
    if (board->angle != 0) {
        D_800E284C->corner[0].x = -halfWidth;
        D_800E284C->corner[0].y = -halfHeight;
        D_800E284C->corner[1].x = halfWidth;
        D_800E284C->corner[1].y = -halfHeight;
        D_800E284C->corner[2].x = -halfWidth;
        D_800E284C->corner[2].y = halfHeight;
        D_800E284C->corner[3].x = halfWidth;
        D_800E284C->corner[3].y = halfHeight;
        D_800E284C->rotation.m[2][2] = 0x1000;
        D_800E284C->rotation.m[1][1] = 0x1000;
        D_800E284C->rotation.m[0][0] = 0x1000;
        D_800E284C->rotation.t[2] = 0;
        D_800E284C->rotation.t[1] = 0;
        D_800E284C->rotation.t[0] = 0;
        D_800E284C->rotation.m[2][1] = 0;
        D_800E284C->rotation.m[2][0] = 0;
        D_800E284C->rotation.m[1][2] = 0;
        D_800E284C->rotation.m[1][0] = 0;
        D_800E284C->rotation.m[0][2] = 0;
        D_800E284C->rotation.m[0][1] = 0;
        RotMatrixZ(board->angle, &D_800E284C->rotation);
        ApplyMatrixSV(&D_800E284C->rotation, &D_800E284C->corner[0],
                      &D_800E284C->turned[0]);
        ApplyMatrixSV(&D_800E284C->rotation, &D_800E284C->corner[1],
                      &D_800E284C->turned[1]);
        ApplyMatrixSV(&D_800E284C->rotation, &D_800E284C->corner[2],
                      &D_800E284C->turned[2]);
        ApplyMatrixSV(&D_800E284C->rotation, &D_800E284C->corner[3],
                      &D_800E284C->turned[3]);
        packet->x0 = D_800E284C->centerX + D_800E284C->turned[0].x;
        packet->y0 = D_800E284C->centerY + D_800E284C->turned[0].y;
        packet->x1 = D_800E284C->centerX + D_800E284C->turned[1].x;
        packet->y1 = D_800E284C->centerY + D_800E284C->turned[1].y;
        packet->x2 = D_800E284C->centerX + D_800E284C->turned[2].x;
        packet->y2 = D_800E284C->centerY + D_800E284C->turned[2].y;
        packet->x3 = D_800E284C->centerX + D_800E284C->turned[3].x;
        packet->y3 = D_800E284C->centerY + D_800E284C->turned[3].y;
    } else {
        packet->x0 = D_800E284C->centerX - halfWidth;
        packet->y0 = D_800E284C->centerY - halfHeight;
        packet->x1 = D_800E284C->centerX + halfWidth;
        packet->y1 = D_800E284C->centerY - halfHeight;
        packet->x2 = D_800E284C->centerX - halfWidth;
        packet->y2 = D_800E284C->centerY + halfHeight;
        packet->x3 = D_800E284C->centerX + halfWidth;
        packet->y3 = D_800E284C->centerY + halfHeight;
    }
    packet->r0 = D_800E284C->rgb[0];
    packet->g0 = D_800E284C->rgb[1];
    packet->b0 = D_800E284C->rgb[2];
    packet->tpage = D_800E27AC;
    packet->clut = GetClut(D_800F341C + D_800E284C->clutX,
                           D_800F341E + D_800E284C->clutY);
    packet->u0 = D_800E284C->u + 1;
    packet->v0 = D_800E284C->v + 1;
    packet->u1 = D_800E284C->u + D_800F345C;
    packet->v1 = D_800E284C->v + 1;
    packet->u2 = D_800E284C->u + 1;
    packet->v2 = D_800E284C->v + D_800F345D;
    packet->u3 = D_800E284C->u + D_800F345C;
    packet->v3 = D_800E284C->v + D_800F345D;
    packet->tag.address = STRIP_OT(D_800E284C->depth + board->depth)->address;
    link.tag = &packet->tag;
    STRIP_OT(D_800E284C->depth + board->depth)->address = link.word;
    D_8009CDD8 += sizeof(FieldStripPacket);
}
