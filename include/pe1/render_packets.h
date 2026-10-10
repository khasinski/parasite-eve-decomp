#ifndef PE1_RENDER_PACKETS_H
#define PE1_RENDER_PACKETS_H

#include "common.h"

/* GPU DMA link: 24-bit next-packet address and an 8-bit command length. */
typedef union RenderPacketTag {
    u32 word;
    struct { u8 address[3], length; } bytes;
    struct { u32 address : 24, length : 8; } link;
} RenderPacketTag;

PE1_STATIC_ASSERT(sizeof(RenderPacketTag) == 4, render_packet_tag_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderPacketTag, bytes.length) == 3,
                  render_packet_tag_length_offset);

/* Variable-size textured sprite packet used by the glyph renderer. */
typedef struct RenderSpritePacket {
    RenderPacketTag tag;
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
    u32 *ordering[2];
    char *other_buffers[6];
    char *packets[2];
} RenderBufferPrefix;

/* Renderer view of the shared game working state. */
typedef struct RenderFrameState {
    u32 flags;
    u8 reserved04[0x15C];
    RenderBufferPrefix buffers;
} RenderFrameState;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderFrameState, buffers) == 0x160, render_frame_buffers_offset);

#endif
