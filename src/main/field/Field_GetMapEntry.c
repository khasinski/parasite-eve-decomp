#include "common.h"
#include "pe1/render_object.h"
#define NULL ((void *)0)

extern struct { char _[16]; } D_8009CDDC_o __asm__("g_ActiveDrawSlot");
#define g_ActiveDrawSlot (*(s32 *)&D_8009CDDC_o)
extern s16 g_RenderPageDeltaTable[4][4] __asm__("D_800921D8");

void Field_GetMapEntry(RenderObjectEntity *arg0, s32 arg1) {
    RenderObjectEntity *t0;
    register s32 t1 asm("$9");
    s32 a3;
    u8 *a2;
    s32 a0;
    s32 a1;
    register s32 v0 asm("$2");
    register void *v1 asm("$3");

    __asm__ volatile(
        "addiu $sp,$sp,-0x10\n\t"
        "addu %0,%1,$zero"
        : "=r"(t0)
        : "r"(arg0)
        : "memory");
    v1 = t0->header;
    a3 = g_ActiveDrawSlot;
    a0 = 0;
    if (v1 != NULL) {
        if (t0->draw_count != 0) {
            v0 = t0->header->packet34_count;
            a2 = t0->primitive_buffer;
            if (v0 != 0) {
                v0 = ((((a3 << 1) + a3) << 2) + a3) << 2;
                goto has_entry;
            } else {
                v0 = t0->header->packet28_count;
                if (v0 != 0) {
                    v0 = ((a3 << 2) + a3) << 3;
has_entry:
                    v0 = (s32) ((u8 *) v0 + (s32) a2);
                    v0 = ((RenderPacketState *)v0)->page_bits;
                    v0 &= 0x7F;
                    a0 = (u32) v0 >> 5;
                }
            }
            v0 = arg1 << 16;
            t1 = v0 >> 16;
            if (t1 != a0) {
                v0 = a0 << 3;
                v1 = g_RenderPageDeltaTable;
                v0 = (s32) ((u8 *)v1 + v0);
                v1 = (void *)(t1 << 1);
                a0 = *(s16 *)((u8 *)v1 + v0);
                v0 = t0->header->packet34_count;
                a1 = 0;
                if (v0 > 0) {
                    v0 = ((((a3 << 1) + a3) << 2) + a3);
                    t1 = v0 << 2;
                    do {
                        v1 = a2 + t1;
                        ((RenderPacketState *)v1)->page_bits =
                            ((RenderPacketState *)v1)->page_bits + a0;
                        v0 = t0->header->packet34_count;
                        __asm__ volatile("" ::: "memory");
                        a1 += 1;
                        a2 += 0x68;
                    } while (a1 < v0);
                }
                v0 = t0->header->packet28_count;
                a1 = 0;
                if (v0 > 0) {
                    v0 = (a3 << 2) + a3;
                    a3 = v0 << 3;
                    do {
                        v1 = a2 + a3;
                        ((RenderPacketState *)v1)->page_bits =
                            ((RenderPacketState *)v1)->page_bits + a0;
                        v0 = t0->header->packet28_count;
                        __asm__ volatile("" ::: "memory");
                        a1 += 1;
                        a2 += 0x50;
                    } while (a1 < v0);
                }
            }
        }
    }
    __asm__ volatile("addiu $sp,$sp,0x10");
}
