#include "pe1/psyq_cd.h"

extern int D_800BE9EC;
extern int D_800C0DBC;

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
    entry = (u8 *)&StRingAddr[index];

    header_reg = header;
    if (*(u16 *)entry == 1) {
        g_StRingIndexPage = (StRingIndexPage *)0x800C0000;
        g_StRingIndexPage[-1].index = 0;
        if (D_800C0DBC != 0) {
            *(u16 *)entry = 0;
        }
        index = D_800BE9EC;
        entry = (u8 *)&StRingAddr[index];
    }

    asm volatile("" : : : "memory");
    if (*(u16 *)entry == 2) {
        *(u16 *)entry = 4;
        *addr_reg = (u32 *)(&StRingAddr[StRingSize] +
                            D_800BE9EC * 0x3F);
        *header_reg = (u32 *)entry;
        return 0;
    }
    return 1;
}
