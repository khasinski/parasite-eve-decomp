/* WIP: score 10, NOT a retail byte-match. See FieldEng_HistoryTrail.md. */
/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/field_history_trail.h"

/* Push the pair (head, tail) onto the `count`-deep history and draw the
 * ribbon through it: quad i joins pairs i and i+1 and fades from
 * (count - i)/count to (count - i - 1)/count of color0 (head side) and
 * color1 (tail side) at scale/128. A missing head repeats the previous
 * pair; count >= 1000 instead clears the first count - 999 pairs. Mode
 * 0xFF draws opaque, anything else semi-transparent. */
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
    register s32 *view asm("$15");
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
    for (i = 0; i < count; i++, history++, packet++) {
        if (history[0].head.vector.pad & history[0].tail.vector.pad & history[1].head.vector.pad &
            history[1].tail.vector.pad) {
            u32 *depthOut = &depth;
            {
                register GteShortVector *v1 asm("$2");
                register GteShortVector *v2 asm("$3");
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
                asm volatile("" : : : "memory");
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
