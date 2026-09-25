#include "pe1/boot_display.h"

void Boot_InitSprite16(BootDisplaySpritePacket *primitive, s16 x, s16 y,
                       u16 u, u16 v) {
    volatile BootDisplaySpritePacket *packet = primitive;

    func_80077C84(&primitive->draw_mode, 0, 0, g_BootDisplayTPage);
    ((u8 *)&primitive->sprite.tag)[3] = 4;
    primitive->sprite.code = 100;
    func_80077B04(&primitive->sprite, 1);
    func_80077CB4(&primitive->draw_mode, &primitive->sprite);
    packet->sprite.w = 16;
    packet->sprite.h = 16;
    packet->sprite.x0 = x;
    packet->sprite.y0 = y;
    packet->sprite.u0 = u;
    packet->sprite.v0 = v;
    packet->sprite.r0 = 255;
    packet->sprite.g0 = 255;
    packet->sprite.b0 = 255;
    packet->sprite.clut = g_BootDisplayClut;
}
