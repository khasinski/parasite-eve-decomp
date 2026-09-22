#ifndef PE1_RENDER_PRIM_H
#define PE1_RENDER_PRIM_H

#include "common.h"
#include "pe1/gte_types.h"

/* A render primitive list and its 0x10-byte color records (Render_SetPrimColour).
 * The stride 0x10 is load-bearing (code indexes entries with i<<4). */
typedef struct PrimEntry {            /* 0x10 */
    unsigned char pad_00[0x04];
    unsigned char r;                  /* 0x04 */
    unsigned char g;                  /* 0x05 */
    unsigned char b;                  /* 0x06 */
    unsigned char pad_07[0x09];
} PrimEntry;

typedef struct PrimObj {
    unsigned char pad_00[0x26];
    unsigned short count;             /* 0x26 */
    unsigned char pad_28[0x08];
    PrimEntry *entries;               /* 0x30 */
} PrimObj;

/* Flat-color GPU line packet: tag, RGB/code, and two screen points. */
typedef struct RenderLinePacket {
    u32 tag;
    u8 r, g, b, code;
    s16 x0, y0, x1, y1;
} RenderLinePacket;

/* Four-vertex textured GPU packet. The final halfwords are packet padding. */
typedef struct RenderTexturedQuad {
    union { u32 word; struct { u8 address[3], length; } bytes; } tag;
    union { u32 word; struct { u8 r, g, b, code; } bytes; } color;
    u16 x0, y0; u8 u0, v0; u16 clut;
    u16 x1, y1; u8 u1, v1; u16 tpage;
    u16 x2, y2; u8 u2, v2; u16 pad2;
    u16 x3, y3; u8 u3, v3; u16 pad3;
} RenderTexturedQuad;

PE1_STATIC_ASSERT(sizeof(RenderTexturedQuad) == 40, render_textured_quad_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderTexturedQuad, clut) == 14, render_quad_clut_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderTexturedQuad, tpage) == 22, render_quad_tpage_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderTexturedQuad, x3) == 32, render_quad_x3_offset);

/* Variable-size textured sprite packet used by the glyph renderer. */
typedef struct RenderSpritePacket {
    union { u32 word; struct { u8 address[3], length; } bytes; } tag;
    union { u32 word; struct { u8 r, g, b, code; } bytes; } color;
    u16 x, y;
    u8 u, v;
    u16 clut, width, height;
} RenderSpritePacket;

PE1_STATIC_ASSERT(sizeof(RenderSpritePacket) == 20, render_sprite_packet_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderSpritePacket, x) == 8, render_sprite_x_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderSpritePacket, clut) == 14, render_sprite_clut_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderSpritePacket, width) == 16, render_sprite_width_offset);

/* Fixed-size textured sprite and texture-page packets built for room tiles.
 * The three-byte address is preserved while the packet length is initialized. */
typedef struct RenderTilePacket {
    u8 address[3], length;
    u8 r, g, b, code;
    u16 x, y;
    u8 u, v;
    u16 clut;
} RenderTilePacket;

typedef struct RenderTexturePagePacket {
    u8 address[3], length;
    u32 command;
} RenderTexturePagePacket;

typedef union RenderTileTexture {
    u32 words[2];
    u8 bytes[8];
} RenderTileTexture;

PE1_STATIC_ASSERT(sizeof(RenderTilePacket) == 16, render_tile_packet_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderTilePacket, clut) == 14, render_tile_clut_offset);
PE1_STATIC_ASSERT(sizeof(RenderTexturePagePacket) == 8, render_texture_page_packet_size);
PE1_STATIC_ASSERT(sizeof(RenderTileTexture) == 8, render_tile_texture_size);
extern u8 D_800BD024;

/* Prefix of the buffer pointers initialized by Boot_InitMemoryLayout.
 * The active draw slot selects one of the two ordering/packet buffers.
 * Six unrelated buffer pointers separate the two pairs. */
typedef struct RenderBufferPrefix {
    char *ordering[2];
    char *other_buffers[6];
    char *packets[2];
} RenderBufferPrefix;
PE1_STATIC_ASSERT(sizeof(RenderLinePacket) == 16, render_line_packet_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderLinePacket, x0) == 8,
                  render_line_packet_points);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderBufferPrefix, packets) == 0x20,
                  render_buffer_prefix_packets);
PE1_STATIC_ASSERT(sizeof(RenderBufferPrefix) == 0x28,
                  render_buffer_prefix_size);
extern RenderBufferPrefix D_800B0E38;
extern int D_8009CDD8, D_8009CDDC;
void SetLineF2(RenderLinePacket *packet);
u16 GetTPage(int tp, int abr, int x, int y);
void SetDrawMode(char *packet, int drawTexture, int dither, int tpage);
void func_800CF6F8(void *ordering, void *packet, int mode);
struct RenderColor;
void func_800DB25C(GteShortVector *position, int x, int y, int angle,
                   int radius, int depth, int intensity, struct RenderColor *color);

#endif /* PE1_RENDER_PRIM_H */
