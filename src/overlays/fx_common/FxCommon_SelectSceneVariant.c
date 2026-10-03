#include "fx_common_setup.h"

/* Pick the effect variant, mirror pairs and texture flag for the current
 * room and area before the shared effect assets are set up. */
void func_80191854(void)
{

    func_8019BD78();
    if (func_8005BCB0() == 0)
        D_8019C008 = 0;
    else
        D_8019C008 = 0xFF;
    if (g_FxCommonGameFlags & 0x40000000) {
        D_8019C008 = 0xFF;
        func_800371A4(1);
    } else {
        D_8019C008 = 0;
        func_800371A4(0);
    }

    D_8019CC52 = -1;
    if (g_FxCommonSceneIds.room == 0x179)
        D_8019CC52 = 6;
    if (g_FxCommonSceneIds.room == 0x18)
        D_8019CC52 = 6;
    if (g_FxCommonSceneIds.room == 0x2E)
        D_8019CC52 = 7;
    if (g_FxCommonSceneIds.room == 0x75)
        D_8019CC52 = 7;
    if (g_FxCommonSceneIds.room == 0x67)
        D_8019CC52 = 7;
    if (g_FxCommonSceneIds.room == 0xBF)
        D_8019CC52 = 9;
    if (g_FxCommonSceneIds.room == 0xC0)
        D_8019CC52 = 9;
    if (g_FxCommonSceneIds.room == 0x5D)
        D_8019CC52 = 2;
    if (g_FxCommonSceneIds.room == 0x60)
        D_8019CC52 = 2;
    if (g_FxCommonSceneIds.room == 0x3A)
        D_8019CC52 = 8;
    if (g_FxCommonSceneIds.room == 0x176)
        D_8019CC52 = 8;
    if (g_FxCommonSceneIds.room == 0x7D)
        D_8019CC52 = 4;
    if (g_FxCommonSceneIds.room == 0x104)
        D_8019CC52 = 3;
    if (g_FxCommonSceneIds.room == 0xA1)
        D_8019CC52 = 1;
    if (g_FxCommonSceneIds.room == 0xB7)
        D_8019CC52 = 0;
    if (g_FxCommonSceneIds.room == 0x122)
        D_8019CC52 = 5;
    if (D_8019CC52 == -1)
        D_8019CC52 = 7;

    if (g_FxCommonSceneIds.area == 0x80)
        D_8019BFF4 = 0;
    if (g_FxCommonSceneIds.area == 0x208)
        D_8019BFF4 = 7;
    if (g_FxCommonSceneIds.area == 0xB8) {
        D_8019C000 = 1;
        D_8019C004 = 9;
    }
    if (g_FxCommonSceneIds.area == 0xC0) {
        D_8019BFF8 = 1;
        D_8019BFFC = 9;
    }
    if (g_FxCommonSceneIds.area == 0xD0) {
        D_8019C000 = 1;
        D_8019C004 = 7;
    }
    if (g_FxCommonSceneIds.area == 0xD8) {
        D_8019BFF8 = 1;
        D_8019BFFC = 7;
    }
    if (g_FxCommonSceneIds.area == 0xE0) {
        D_8019C000 = 1;
        D_8019C004 = 8;
    }
    if (g_FxCommonSceneIds.area == 0xE4) {
        D_8019BFF8 = 1;
        D_8019BFFC = 8;
    }
    if (g_FxCommonSceneIds.area == 0x148) {
        D_8019C000 = 1;
        D_8019C004 = 9;
    }
    if (g_FxCommonSceneIds.area == 0x160) {
        D_8019BFF8 = 1;
        D_8019BFFC = 9;
    }
    if (g_FxCommonSceneIds.area == 0x178) {
        D_8019C000 = 1;
        D_8019C004 = 7;
    }
    if (g_FxCommonSceneIds.area == 0x180) {
        D_8019BFF8 = 1;
        D_8019BFFC = 7;
    }
    if (g_FxCommonSceneIds.area == 0x1C0) {
        D_8019C000 = 1;
        D_8019C004 = 4;
    }
    if (g_FxCommonSceneIds.area == 0x1C8) {
        D_8019BFF8 = 1;
        D_8019BFFC = 4;
    }
    if (g_FxCommonSceneIds.flags & 0x2000)
        D_8019C1F0 = 1;
    else
        D_8019C1F0 = 0;
    func_80191C94();
}
