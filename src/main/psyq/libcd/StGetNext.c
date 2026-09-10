#include "common.h"

extern int D_800BE9EC;
extern int D_800C0DBC;
extern int D_800C20C4;
extern u8 *D_800C0DC8;

typedef struct StRingIndexPage {
    int index;
    char reserved04[0x1610];
} StRingIndexPage;

register StRingIndexPage *g_StRingIndexPage asm("$1");

u32 StGetNext(u32 **addr, u32 **header) {
    u32 **addr_reg;
    u32 **header_reg;
    u8 *entry;
    int index;

    addr_reg = addr;
    index = D_800BE9EC;
    entry = D_800C0DC8 + (index << 5);

    header_reg = header;
    if (*(u16 *)entry == 1) {
        g_StRingIndexPage = (StRingIndexPage *)0x800C0000;
        g_StRingIndexPage[-1].index = 0;
        if (D_800C0DBC != 0) {
            *(u16 *)entry = 0;
        }
        index = D_800BE9EC;
        entry = D_800C0DC8 + (index << 5);
    }

    asm volatile("" : : : "memory");
    if (*(u16 *)entry == 2) {
        *(u16 *)entry = 4;
        *addr_reg = (u32 *)(D_800C0DC8 + (D_800C20C4 << 5) + (((D_800BE9EC << 6) - D_800BE9EC) << 5));
        *header_reg = (u32 *)entry;
        return 0;
    }
    return 1;
}
