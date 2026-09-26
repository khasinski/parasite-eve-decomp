#include "pe1/boot_display.h"

void Boot_InitDisplayBuffers(void) {
    s16 rect[4];
    u16 line;
    /* Keep the original buffer offset in v0 across both address calculations. */
    register s32 delta asm("$2");
    char *base;
    char *start;
    char *end;
    s32 offset;
    u16 *screenY;
    void *resource;

    func_80074A44(1);
    func_80074BB8(0);
    func_80074D28(0);

    screenY = &D_80093168;
    g_BootDisplayTextScrollX = 0xE000;
    g_BootDisplayTextScrollStepX = -0x100;
    rect[0] = 0;
    rect[1] = 0;
    rect[2] = 0x3FF;
    rect[3] = 0x1FF;
    g_BootDisplayBufferIndex = 0;
    g_BootDisplayState = 0;
    g_BootDisplayTransferProgress = 0;
    g_BootDisplayStateTimer = 0;
    g_BootDisplayStateTimer = 0;

    line = *screenY;
    base = D_80011610;
    delta = (line - D_80093166) << 11;
    start = base + delta;
    delta += 0x54000;
    end = D_80011610 + delta;
    D_80125B88[0] = start;
    D_80125B90 = end;
    D_80125B98 = end;
    D_80125BA0 = start;
    func_80074F44(rect, 0, 0, 0);
    func_80074DC0(0);
    func_8010BE3C(0);
    offset = 0x43000;
    func_8010BD4C(((*screenY - D_80093166) << 11) + offset + D_80011610);

    resource = D_8012682C;
    D_80172C98 = func_80071994(resource);
    D_80172C9C = func_800719C4(resource);
    D_80172C90 = func_80071964(resource);
    D_80172C94 = func_80071944(resource);
    g_BootDisplayTPage = func_80077A64(0, 0, D_80172C90[0], D_80172C90[1]);
    g_BootDisplayClut = func_80077AA4(D_80172C94[0], D_80172C94[1]);
    func_80124F40(0);
    func_80124F40(1);
    func_800755F0(g_RenderDisplayEnvArray + (g_ActiveDrawSlot * 20));
    func_80074D28(1);
}
