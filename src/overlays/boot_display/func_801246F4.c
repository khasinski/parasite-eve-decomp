#include "common.h"
extern s32 g_ActiveDrawSlot __asm__("D_8009CDDC");
extern char g_RenderDisplayEnvArray[] __asm__("D_800BCE80");
extern char g_RenderDrawEnvArray[] __asm__("D_800BCDC8");
extern char *g_BootDisplayOrderingTable;

extern void func_80073A44(s32 value);
extern void func_80074A44(s32 value);
extern void func_80074DC0(s32 value);
extern void func_800754E4(void *dst, void *src);
extern void func_800755F0(void *entry);

void func_801246F4(void) {
    func_80074DC0(0);
    func_80073A44(2);
    func_80074A44(1);
    func_800755F0(g_RenderDisplayEnvArray + (g_ActiveDrawSlot * 20));
    func_800754E4(g_BootDisplayOrderingTable + 0x3C,
                  g_RenderDrawEnvArray + ((((g_ActiveDrawSlot * 3) << 3) - g_ActiveDrawSlot) << 2));
    g_ActiveDrawSlot ^= 1;
}
