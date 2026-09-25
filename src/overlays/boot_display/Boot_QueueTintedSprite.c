#include "pe1/boot_display.h"

void Boot_QueueTintedSprite(u16 x, u16 y, u8 color) {
    char *primitive = g_BootDisplayPrimitiveCursor;
    s32 packed = func_80123B1C(color);

    func_8012462C(primitive, x, y, (packed & 15) << 4, packed & 0xF0);
    func_80077AC4(g_BootDisplayOrderingTable + 0x2C, primitive);
    g_BootDisplayPrimitiveCursor += 0x1C;
}
