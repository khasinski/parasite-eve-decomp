#include "common.h"
extern s32 g_ActiveDrawSlot __asm__("D_8009CDDC");
extern u16 D_80093168, D_80093166;
extern char *D_80011610;
extern char *volatile g_BootDisplayOrderingTable;
extern char *volatile g_BootDisplayPrimitiveCursor;
extern void func_800752AC(void *, s32);
void Boot_SelectFrameBuffers(void) {
    register s32 orderingOffset asm("$4");
    s32 primitiveOffset;
    register s32 sectorOffset;
    if (g_ActiveDrawSlot != 0) {
        orderingOffset = 0x581E0;
        primitiveOffset = 0x560D0;
        sectorOffset = (D_80093168 - D_80093166) << 11;
    } else {
        orderingOffset = 0x581A0;
        primitiveOffset = 0x54000;
        sectorOffset = (D_80093168 - D_80093166) << 11;
    }
    orderingOffset = sectorOffset + orderingOffset;

    {
        register char *base asm("$3") = D_80011610;
        register char *orderingBuffer;
        sectorOffset += primitiveOffset;
        orderingBuffer = base + orderingOffset;
        base = base + sectorOffset;
        g_BootDisplayOrderingTable = orderingBuffer;
        g_BootDisplayPrimitiveCursor = base;
    }
    func_800752AC(g_BootDisplayOrderingTable, 16);
}
