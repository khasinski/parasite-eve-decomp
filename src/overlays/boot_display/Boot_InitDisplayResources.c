#include "pe1/boot_display.h"

void Boot_InitDisplayResources(void) {
    s16 rect[4];
    char *resource;
    s32 offset;

    func_80074A44(1);
    func_80074BB8(0);
    func_80074D28(0);

    g_BootDisplayTextScrollX = 0xE000;
    g_BootDisplayTextScrollStepX = -0x100;
    g_BootDisplayTransition = 1;
    rect[2] = 0x3FF;
    g_BootDisplayStateTimer = 0;
    rect[0] = 0;
    rect[1] = 0;
    rect[3] = 0x1FF;
    g_BootDisplayBufferIndex ^= 1;
    func_80074F44(rect, 0, 0, 0);
    func_80074DC0(0);

    resource = D_8012682C;
    func_800718D0(resource);
    D_80172C98 = func_800718D0(resource);
    D_80172C9C = func_800719C4(resource);
    D_80172C90 = func_80071964(resource);
    D_80172C94 = func_80071944(resource);
    g_BootDisplayTPage = func_80077A64(0, 0, D_80172C90[0], D_80172C90[1]);
    g_BootDisplayClut = func_80077AA4(D_80172C94[0], D_80172C94[1]);
    func_8010BE3C(0);
    offset = 0x43000;
    func_8010BD4C(D_80011610 + (((D_80093168 - D_80093166) << 11) + offset));
    func_80074D28(1);
}
