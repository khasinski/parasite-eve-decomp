#ifndef PE1_DRAW_BUFFERS_H
#define PE1_DRAW_BUFFERS_H

#include "common.h"
#include "pe1/psyq_gpu.h"

/* Game double-buffer records: SDK environments followed by the ordering-table
 * and packet-arena pointers. Selection advances by one 0x78-byte record. */
typedef struct DrawFrameBuffer {
    DRAWENV draw;
    DISPENV display;
    u32 *orderingTableBase;
    u8 *frontBufferBase;
} DrawFrameBuffer;

PE1_STATIC_ASSERT(sizeof(DrawFrameBuffer) == 0x78, draw_frame_buffer_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DrawFrameBuffer, display) == 0x5C,
                  draw_frame_buffer_display_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DrawFrameBuffer, orderingTableBase) == 0x70,
                  draw_frame_buffer_ordering_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DrawFrameBuffer, frontBufferBase) == 0x74,
                  draw_frame_buffer_packets_offset);
extern DrawFrameBuffer D_800A2180[];
extern DrawFrameBuffer *D_8009D0FC;
extern u32 *D_8009D118;

#endif
