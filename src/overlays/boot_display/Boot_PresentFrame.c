#include "pe1/boot_display.h"
void Boot_PresentFrame(void) {
    s16 rect[4];
    u16 timer;
    if (g_ActiveDrawSlot != 0) {
        rect[2] = 480;
        rect[0] = 0;
        rect[1] = 0;
        rect[3] = 224;
    } else {
        rect[0] = 0;
        rect[1] = 224;
        rect[2] = 480;
        rect[3] = 224;
    }
    func_8007506C(rect, D_80125B88[g_BootDisplayBufferIndex * 4]);
    if (g_BootDisplayState == 6) {
        timer = g_BootDisplayStateTimer + 1;
        g_BootDisplayStateTimer = timer;
        g_BootDisplayBufferIndex ^= 1;
        if (timer < 31)
            g_BootDisplayState = 0;
        else
            g_BootDisplayState = 7;
    }
    func_80074DC0(0);
    func_80073A44(2);
    func_800755F0(g_RenderDisplayEnvArray + g_ActiveDrawSlot * 20);
    func_80075424(g_RenderDrawEnvArray + g_ActiveDrawSlot * 92);
    g_ActiveDrawSlot ^= 1;
}
