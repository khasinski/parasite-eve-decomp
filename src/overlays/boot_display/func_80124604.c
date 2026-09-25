#include "pe1/boot_display.h"

void func_80124604(s32 color) {
    BootDisplaySpritePacket *primitive = (BootDisplaySpritePacket *)g_BootDisplayPrimitiveCursor;

    func_80077C84(&primitive->draw_mode, 0, 0, g_BootDisplayTPage);
    ((u8 *)&primitive->sprite.tag)[3] = 4;
    primitive->sprite.code = 100;
    func_80077B04(&primitive->sprite, 1);
    func_80077CB4(&primitive->draw_mode, &primitive->sprite);

    primitive->sprite.x0 = 120;
    primitive->sprite.y0 = 110;
    primitive->sprite.u0 = 0;
    primitive->sprite.v0 = 0x80;
    primitive->sprite.r0 = (u8)color;
    primitive->sprite.g0 = (u8)color;
    primitive->sprite.b0 = (u8)color;
    primitive->sprite.w = 0x60;
    primitive->sprite.h = 0x18;
    primitive->sprite.clut = g_BootDisplayClut;
    func_80077AC4(g_BootDisplayOrderingTable + 0x2C, primitive);
    g_BootDisplayPrimitiveCursor += 0x1C;
}
