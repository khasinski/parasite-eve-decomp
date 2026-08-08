#include "common.h"
typedef struct {
    s32 tag;
    s32 code;
} DrawModePrim;

typedef struct {
    s32 tag;
    u8 r0;
    u8 g0;
    u8 b0;
    u8 code;
    u16 x0;
    u16 y0;
    u8 u0;
    u8 v0;
    u16 clut;
    u16 w;
    u16 h;
} SpritePrim;

typedef struct {
    DrawModePrim draw_mode;
    SpritePrim sprite;
} BootSpritePrim;

extern char *g_BootDisplayOrderingTable;
extern u16 g_BootDisplayTPage;
extern u16 g_BootDisplayClut;
extern char *g_BootDisplayPrimitiveCursor;

extern void func_80077AC4(void *ordering_table, void *primitive);
extern void func_80077B04(void *primitive, s32 code);
extern void func_80077C84(void *primitive, s32 x, s32 y, u16 tpage);
extern void func_80077CB4(void *primitive, void *next);

void func_80124604(s32 color) {
    BootSpritePrim *primitive = (BootSpritePrim *)g_BootDisplayPrimitiveCursor;

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
