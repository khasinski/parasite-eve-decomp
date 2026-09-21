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
void func_800CF6F8(void *ordering, void *packet, int depth);
struct RenderColor;
void func_800DB25C(GteShortVector *position, int x, int y, int angle,
                   int radius, int depth, int intensity, struct RenderColor *color);

#endif /* PE1_RENDER_PRIM_H */
