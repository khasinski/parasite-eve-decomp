/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"

/* Each textbox entry is 0x38 bytes; preserve the retail field addresses. */
extern u8 D_800BCEA8[];
extern u8 D_800BCEAC[];
extern u8 D_800BCEB0[];
extern u8 D_800BCEB1[];
extern u8 D_800BCEB4[];
extern u8 D_800BCEB5[];
extern u16 D_800BCEB8[];
extern u8 D_8009EC86[];
extern u8 D_8009EC78[];
extern u8 D_8009EC70[];
extern u8 D_8009ECA8[];
extern u16 D_80091680[];
extern u8 D_8009CEA0;
extern u8 D_8009CED0;
extern int D_8009CED4;
extern void *D_8009CE90;
void SetDrawMode(void *, int, int, int);
void SetSprt(void *);
int Gpu_AppendPacketPrimitive(void *, void *);
void Gpu_SetDrawEnable(void *, int);
int GetTPage(int, int, int, int);
void SetTile(void *);
void Gpu_SetDither(void *, int);
void exit(int);

void Render_SetupFogLayer(void *source) {
    int i;
    u16 *page_source;
    u8 *tile_base;
    register u8 *draw_base asm("$23");
    u8 *tile_payload_base;
    int sentinel;
    register u8 *sprite_base asm("$11");
    int offset;
    u8 *draw_mode;
    u8 *sprite;
    u8 *sprite_fields;
    u8 *tile_mode;
    u8 *tile;
    u8 *payload;
    u32 flags;
    int tpage;
    int color;
    int zero_arg;
    int x_arg;
    int y_arg;
    int dither_enabled;
    u32 mask1;
    u32 mask2;
    u32 mask3;
    u32 mask4;
    u32 mask5;
    i = 0;
    mask1 =0xFFF0FFFF;
    mask2 =0xFFEFFFFF;
    mask3 =0xFFDFFFFF;
    mask4 =0xFE3FFFFF;
    mask5 =0xFDFFFFFF;
    sentinel = -1;
    for (; (u8)i < 4; ++i) {
        offset = ((((u8)i << 3) - (u8)i) << 3);
        *(u8 *)(D_800BCEB4 + offset) = 0;
        *(u8 *)(D_800BCEB5 + offset) = 0;
        asm volatile("" ::: "memory");
        flags = *(u32 *)(D_800BCEB4 + offset);
        *(u8 *)(D_800BCEA8 + offset) = 0;
        *(u32 *)(D_800BCEAC + offset) = 0;
        *(u8 *)(D_800BCEB0 + offset) = 0;
        *(u8 *)(D_800BCEB1 + offset) = 0;
        *(s16 *)((u8 *)D_800BCEB8 + offset) = sentinel;
        flags &= mask1;
        flags &= mask2;
        flags &= mask3;
        flags &= mask4;
        flags &= mask5;
        *(u32 *)(D_800BCEB4 + offset) = flags;
    }
    i = 0;
    page_source = D_80091680;
    draw_base = D_8009EC70;
    tile_base = D_8009ECA8;
    tile_payload_base = tile_base + 8;
    D_8009CEA0 = 0;
    D_8009CED0 = 0;
    D_8009CED4 = 0;
    D_8009CE90 = source;
    for (; (u8)i < 2; ++i) {
        draw_mode = (u8 *)((int)((u8)i * 28) + (int)draw_base);
        sprite = draw_mode + 8;
        SetDrawMode(draw_mode, 0, 1, page_source[0]);
        SetSprt(sprite);
        if (Gpu_AppendPacketPrimitive(draw_mode, sprite)) exit(-1);
        sprite_base = D_8009EC78;

        sprite_fields = (u8 *)((int)((u8)i * 28) + (int)sprite_base);
        Gpu_SetDrawEnable(sprite_fields, 1);
        x_arg = 0;

        y_arg = 0;

        zero_arg = 0;

        sprite_fields[0xC] = 0x70;
        color = *(page_source - 3);
        *(u16 *)(sprite_fields + 0x10) = 0x18;
        *(u16 *)(sprite_fields + 0x12) = 0xC;
        sprite_fields[0xD] = color;
        *(u16 *)(D_8009EC86 + (u8)i * 28) = page_source[1];
        tpage = GetTPage(x_arg, y_arg, zero_arg, 0);
        asm volatile("" ::: "memory");
        tile_mode = (u8 *)((int)((u8)i * 24) + (int)tile_base);
        tile = tile_mode + 8;
        SetDrawMode(tile_mode, 0, 1, tpage & 0xFFFF);
        SetTile(tile);
        if (Gpu_AppendPacketPrimitive(tile_mode, tile)) exit(-1);
        payload = (u8 *)((int)((u8)i * 24) + (int)tile_payload_base);
        asm volatile("" : "=r"(payload) : "0"(payload));
        dither_enabled = 1;
        asm volatile("" : "=r"(dither_enabled) : "0"(dither_enabled));
        {
            int two = 2;

            payload[4] = two;
            payload[5] = two;
            payload[6] = two;
            *(u16 *)(payload + 0xC) = 0x140;
            *(u16 *)(payload + 0xE) = 0x36;
            *(u16 *)(payload + 8) = 0;
            *(u16 *)(payload + 0xA) = 0xAA;
            Gpu_SetDither(payload, dither_enabled);
        }
    }
}
