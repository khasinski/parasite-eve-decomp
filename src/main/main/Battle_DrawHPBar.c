typedef unsigned char u8;
typedef unsigned short u16;

extern u8 D_8009CD90[];
extern u8 D_800BE9F0[];
extern u8 D_800B01C0[];
extern u8 D_8009E068[];
extern u8 D_8009E098[];
extern u8 D_800B00E8[];
extern u8 D_800B6920[];
extern u8 D_8009E0F0[];
extern u8 D_8009E1D0[];
extern u8 D_800B0130[];
extern u8 D_8009E0B8[];
extern u8 D_8009E2E8[];
extern u8 D_8009E320[];
extern u8 D_8009E358[];
extern u8 D_8009E460[];
extern u8 D_8009E498[];
extern u8 D_8009E4D8[];
extern u8 D_8009E3B8[];
extern u8 D_8009E500[];
extern u8 D_8009E768[];
extern u8 D_8009E7A0[];
extern u8 D_8009E730[];
extern u8 D_8009E880[];
extern u8 D_8009E8B8[];
extern u8 D_8009E928[];
extern u8 D_8009E960[];
extern u8 D_8009EC38[];

u16 GetTPage(int tp, int abr, int x, int y);
u16 GetClut(int x, int y);
u8 *Draw_LookupGlyphDescriptor(int index);
void Gpu_InitDrawModeSprtPacket(void *packet, int tpage);
void Gpu_InitDrawModeTilePacket(void *packet, int tpage);
void Gpu_SetDither(u8 *packet, int enabled);
void Gpu_SetDrawEnable(u8 *packet, int enabled);
void SetLineF2(u8 *prim);
void SetPolyF3(u8 *prim);
void SetPolyFT4(u8 *prim);
void SetPolyG4(u8 *prim);
void SetTile(u8 *prim);

#define U16_AT(ptr, off) (*(u16 *)((ptr) + (off)))

static int Battle_Buffer28Index(int buffer) {
    return (((buffer << 3) - buffer) << 2);
}

static int Battle_Buffer56Index(int buffer) {
    return (((buffer << 3) - buffer) << 3);
}

static int Battle_Buffer72Index(int buffer) {
    return (((buffer << 3) + buffer) << 3);
}

static int Battle_Buffer140Index(int buffer) {
    return (((((buffer << 3) + buffer) << 2) - buffer) << 2);
}

static int Battle_Buffer280Index(int buffer) {
    return (((((buffer << 3) + buffer) << 2) - buffer) << 3);
}

static int Battle_Buffer1400Index(int buffer) {
    return ((((((buffer << 1) + buffer) << 2) - buffer) << 4) - buffer) << 3;
}

static int Battle_BufferDigitIndex(int buffer) {
    return ((buffer << 2) + buffer) << 3;
}

static void Battle_SetRgb(u8 *prim, int r, int g, int b) {
    prim[4] = (u8)r;
    prim[5] = (u8)g;
    prim[6] = (u8)b;
}

static void Battle_SetFt4TexCoords(u8 *prim, u8 *glyph) {
    prim[0x0C] = glyph[0];
    prim[0x0D] = glyph[1];
    prim[0x14] = glyph[0] + glyph[4];
    prim[0x15] = glyph[1];
    prim[0x1C] = glyph[0];
    prim[0x1D] = glyph[1] + glyph[5];
    prim[0x24] = glyph[0] + glyph[4];
    prim[0x25] = glyph[1] + glyph[5];
    U16_AT(prim, 0x0E) = glyph[2];
}

static void Battle_ClearFt4Placement(u8 *prim) {
    U16_AT(prim, 0x08) = 0;
    U16_AT(prim, 0x0A) = 0;
    U16_AT(prim, 0x10) = 0;
    U16_AT(prim, 0x12) = 0;
    U16_AT(prim, 0x18) = 0;
    U16_AT(prim, 0x1A) = 0;
    U16_AT(prim, 0x20) = 0;
    U16_AT(prim, 0x22) = 0;
}

static void Battle_InitGlyphFt4(int buffer, int glyph_id, int clut) {
    u8 *glyph;
    u8 *prim;
    int offset;

    glyph = Draw_LookupGlyphDescriptor(glyph_id);
    offset = Battle_BufferDigitIndex(buffer);
    prim = D_800BE9F0 + offset;
    SetPolyFT4(prim);
    Battle_SetFt4TexCoords(prim, glyph);
    U16_AT(prim, 0x16) = GetTPage(0, 0, 0x1C0, 0);
    Battle_ClearFt4Placement(prim);
    Battle_SetRgb(prim, 0, 0, 0);
    U16_AT(prim, 0x0E) = glyph[2];
    Gpu_SetDither(prim, 1);
}

static void Battle_InitSpriteGrid(u8 *base, int buffer, int outer_count, int inner_count, int clut, int width, int height,
                                  int scale) {
    int outer;
    int inner;
    int buffer_base;
    u8 *packet;

    buffer_base = Battle_Buffer1400Index(buffer) / scale;
    for (outer = 0; outer < outer_count; outer++) {
        for (inner = 0; inner < inner_count; inner++) {
            packet = base + buffer_base + outer * 140 + inner * 28;
            Gpu_InitDrawModeSprtPacket(packet, GetTPage(0, 1, 0x100, 0x1E0));
            U16_AT(packet + 8, 0x0E) = clut;
            U16_AT(packet + 8, 0x08) = width;
            U16_AT(packet + 8, 0x0A) = height;
        }
    }
}

static void Battle_InitSpriteRun(u8 *base, int buffer, int count, int clut, int width, int height, int stride_scale) {
    int i;
    int buffer_base;
    u8 *packet;

    buffer_base = Battle_Buffer140Index(buffer) * stride_scale;
    for (i = 0; i < count; i++) {
        packet = base + buffer_base + i * 28;
        Gpu_InitDrawModeSprtPacket(packet, GetTPage(0, 1, 0x100, 0x1E0));
        U16_AT(packet + 8, 0x0E) = clut;
        U16_AT(packet + 8, 0x08) = width;
        U16_AT(packet + 8, 0x0A) = height;
        Battle_SetRgb(packet + 8, 0x80, 0x80, 0x80);
    }
}

static void Battle_InitDrawModeSprite(u8 *base, int buffer, int clut, int u, int v, int w, int h) {
    u8 *packet;

    packet = base + Battle_Buffer28Index(buffer);
    Gpu_InitDrawModeSprtPacket(packet, GetTPage(0, 1, 0x100, 0x1E0));
    packet += 8;
    packet[0x0C] = (u8)u;
    packet[0x0D] = (u8)v;
    U16_AT(packet, 0x0E) = clut;
    U16_AT(packet, 0x10) = (u16)w;
    U16_AT(packet, 0x12) = (u16)h;
    Battle_SetRgb(packet, 0x80, 0x80, 0x80);
}

static void Battle_InitTilePacket(u8 *base, int buffer, int tpage, int r, int g, int b, int w, int h) {
    u8 *packet;

    packet = base + (((buffer << 1) + buffer) << 3);
    Gpu_InitDrawModeTilePacket(packet, tpage);
    packet += 8;
    Battle_SetRgb(packet, r, g, b);
    U16_AT(packet, 0x08) = w;
    U16_AT(packet, 0x0A) = h;
    Gpu_SetDither(packet, 1);
}

static void Battle_InitGradientPair(int buffer, int clut) {
    u8 *left;
    u8 *right;
    int base;

    base = Battle_Buffer72Index(buffer);
    left = D_800B0130 + base;
    right = left + 0x24;
    SetPolyG4(left);
    SetPolyG4(right);

    left[4] = 0;
    left[5] = 0x82;
    left[6] = 0x36;
    left[0x0C] = 0x4A;
    left[0x0D] = 0xFF;
    left[0x0E] = 0x3B;
    left[0x14] = 0;
    left[0x15] = 0x82;
    left[0x16] = 0x36;
    left[0x1C] = 0x4A;
    left[0x1D] = 0xFF;
    left[0x1E] = 0x3B;

    right[4] = 0xFF;
    right[5] = 0x3D;
    right[6] = 0x81;
    right[0x0C] = 0x83;
    right[0x0D] = 0x13;
    right[0x0E] = 1;
    right[0x14] = 0xFF;
    right[0x15] = 0x3D;
    right[0x16] = 0x81;
    right[0x1C] = 0x83;
    right[0x1D] = 0x13;
    right[0x1E] = 1;

    Battle_InitDrawModeSprite(D_8009E0B8, buffer, clut, 0, 0, 0, 0);
}

static void Battle_InitLineAndTriangle(int buffer, int clut) {
    int base28;
    int base32;
    u8 *packet;
    u8 *line;
    u8 *tri;

    base28 = Battle_Buffer28Index(buffer);
    packet = D_8009E460 + base28;
    Gpu_InitDrawModeSprtPacket(packet, GetTPage(0, 1, 0x100, 0x1E0));
    packet += 8;
    packet[0x0C] = 0xE8;
    packet[0x0D] = 0xE0;
    U16_AT(packet, 0x0E) = clut;
    U16_AT(packet, 0x10) = 0x18;
    U16_AT(packet, 0x12) = 0x18;

    base32 = buffer << 5;
    line = D_8009E498 + base32;
    SetLineF2(line);
    SetLineF2(line + 0x10);
    Battle_SetRgb(line, 0xE0, 0xE0, 0xE0);
    Battle_SetRgb(line + 0x10, 0x60, 0x60, 0x60);

    tri = D_8009E4D8 + (((buffer << 2) + buffer) << 2);
    SetPolyF3(tri);
}

static void Battle_InitTinyTileRun(int buffer) {
    int i;
    int base;
    u8 *tile;

    base = ((buffer << 1) + buffer) << 4;
    for (i = 0; i < 3; i++) {
        tile = D_8009E358 + base + (i << 4);
        SetTile(tile);
        Battle_SetRgb(tile, D_8009CD90[i], D_8009CD90[i], D_8009CD90[i]);
    }
}

static void Battle_InitGlyphDigits(int buffer) {
    int i;
    int base;
    u8 *glyph;
    u8 *packet;

    Gpu_InitDrawModeSprtPacket(D_8009E928 + Battle_Buffer28Index(buffer), GetTPage(0, 1, 0x100, 0x1E0));
    packet = D_8009E928 + Battle_Buffer28Index(buffer) + 8;
    Battle_SetRgb(packet, 0x80, 0x80, 0);

    base = Battle_Buffer280Index(buffer);
    for (i = 0; i < 13; i++) {
        glyph = Draw_LookupGlyphDescriptor(i + 0x6A);
        packet = D_8009E960 + base + i * 28;
        Gpu_InitDrawModeSprtPacket(packet, GetTPage(0, 0, 0x1C0, 0));
        packet += 8;
        packet[0x0C] = glyph[0];
        packet[0x0D] = glyph[1];
        U16_AT(packet, 0x0E) = glyph[2];
        U16_AT(packet, 0x10) = glyph[4];
        U16_AT(packet, 0x12) = glyph[5];
        Battle_SetRgb(packet, 0x80, 0x80, 0x80);
    }
}

static void Battle_InitStatusIconBlock(int buffer, int clut) {
    int base;

    base = Battle_Buffer28Index(buffer);
    Battle_InitDrawModeSprite(D_8009E768, buffer, clut, 0x58, 0xEF, 0x24, 5);
    Battle_InitSpriteRun(D_8009E7A0, buffer, 4, clut, 6, 6, 2);

    Gpu_InitDrawModeSprtPacket(D_8009E730 + base, GetTPage(0, 1, 0x100, 0x1E0));
    D_8009E730[base + 8 + 0x0C] = 0x68;
    D_8009E730[base + 8 + 0x0D] = 0xF4;
    U16_AT(D_8009E730 + base + 8, 0x0E) = GetClut(0x130, 0x1F9);
    U16_AT(D_8009E730 + base + 8, 0x10) = 0x18;
    U16_AT(D_8009E730 + base + 8, 0x12) = 4;
    Battle_SetRgb(D_8009E730 + base + 8, 0x80, 0x80, 0x80);

    Battle_InitDrawModeSprite(D_8009E880, buffer, clut, 0x7C, 0xEF, 0x24, 5);
    Battle_InitSpriteRun(D_8009E8B8, buffer, 2, clut, 6, 6, 2);
}

void Battle_DrawHPBar(void) {
    int buffer;
    int clut;
    int tpage;

    tpage = GetTPage(0, 1, 0x100, 0x1E0);
    clut = GetClut(0x130, 0x1F8);

    for (buffer = 0; buffer < 2; buffer++) {
        Battle_InitGlyphFt4(buffer, 0x8B, clut);
        Battle_InitSpriteGrid(D_800B01C0, buffer, 10, 4, clut, 0, 0x0A, 1);
        Battle_InitTilePacket(D_8009E068, buffer, GetTPage(0, 0, 0, 0), 0x30, 0x30, 0x30, 0, 0);

        SetTile(D_8009E098 + (buffer << 4));
        Battle_SetRgb(D_8009E098 + (buffer << 4), 0x1D, 0x3E, 0x32);
        U16_AT(D_8009E098 + (buffer << 4), 0x0C) = 0x38;
        U16_AT(D_8009E098 + (buffer << 4), 0x0E) = 3;

        Battle_InitGradientPair(buffer, clut);
        Battle_InitDrawModeSprite(D_800B6920, buffer, clut, 0xC8, 0xE0, 4, 8);
        D_800B00E8[Battle_Buffer28Index(buffer) + 6] = 0x82;
        D_800B00E8[Battle_Buffer28Index(buffer) + 0x0D] = 0xFF;
        D_800B00E8[Battle_Buffer28Index(buffer) + 0x16] = 0x82;
        D_800B00E8[Battle_Buffer28Index(buffer) + 0x1D] = 0xFF;

        Battle_InitSpriteRun(D_8009E0F0, buffer, 4, clut, 6, 0x0A, 2);
        Battle_InitSpriteRun(D_8009E1D0, buffer, 5, clut, 6, 0x0A, 2);

        Battle_InitDrawModeSprite(D_8009E2E8, buffer, clut, 0x50, 0xF4, 8, 4);
        Gpu_SetDrawEnable(D_8009E2E8 + Battle_Buffer28Index(buffer) + 8, 1);
        Battle_InitDrawModeSprite(D_8009E320, buffer, clut, 0x58, 0xF4, 8, 4);
        Gpu_SetDrawEnable(D_8009E320 + Battle_Buffer28Index(buffer) + 8, 1);
        Battle_InitDrawModeSprite(D_8009E320 + 0x38, buffer, clut, 0x60, 0xF4, 8, 4);

        Battle_InitTinyTileRun(buffer);
        Battle_InitLineAndTriangle(buffer, clut);
        Battle_InitSpriteRun(D_8009E3B8, buffer, 3, clut, 0x18, 8, 2);
        Battle_InitSpriteRun(D_8009E500, buffer, 10, clut, 0, 0, 4);
        Battle_InitStatusIconBlock(buffer, clut);
        Battle_InitGlyphDigits(buffer);
        Battle_InitDrawModeSprite(D_8009EC38, buffer, GetTPage(0, 0, 0x1C0, 0), 0, 0, 0x10, 0x10);
    }
}
