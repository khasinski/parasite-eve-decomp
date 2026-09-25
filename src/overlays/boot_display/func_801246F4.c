#include "pe1/boot_display.h"

void func_801246F4(void) {
    func_80074DC0(0);
    func_80073A44(2);
    func_80074A44(1);
    func_800755F0(g_RenderDisplayEnvArray + (g_ActiveDrawSlot * 20));
    func_800754E4(g_BootDisplayOrderingTable + 0x3C,
                  g_RenderDrawEnvArray + ((((g_ActiveDrawSlot * 3) << 3) - g_ActiveDrawSlot) << 2));
    g_ActiveDrawSlot ^= 1;
}
