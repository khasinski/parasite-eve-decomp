#ifndef PE1_RENDER_PACKETS_H
#define PE1_RENDER_PACKETS_H

#include "common.h"

/* Variable-size textured sprite packet used by the glyph renderer. */
typedef struct RenderSpritePacket {
    union {
        u32 word;
        struct {
            u8 address[3], length;
        } bytes;
    } tag;
    union {
        u32 word;
        struct {
            u8 r, g, b, code;
        } bytes;
    } color;
    u16 x, y;
    u8 u, v;
    u16 clut, width, height;
} RenderSpritePacket;

PE1_STATIC_ASSERT(sizeof(RenderSpritePacket) == 20, render_sprite_packet_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderSpritePacket, x) == 8, render_sprite_x_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderSpritePacket, clut) == 14, render_sprite_clut_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderSpritePacket, width) == 16, render_sprite_width_offset);

/* Prefix of the buffer pointers initialized by Boot_InitMemoryLayout.
 * The active draw slot selects one of the two ordering/packet buffers.
 * Six unrelated buffer pointers separate the two pairs. */
typedef struct RenderBufferPrefix {
    char *ordering[2];
    char *other_buffers[6];
    char *packets[2];
} RenderBufferPrefix;

#endif
