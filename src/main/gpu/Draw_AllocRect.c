#include "common.h"
#include "pe1/draw_level_bar.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "m2c_macros.h"

#include "pe1/psyq_gpu.h"
#include "pe1/draw_area.h"

extern s32 g_DrawBufferIndex;

void Draw_AllocPrimRectFull(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    DrawAreaRect rect;
    u8 *oldPacket;
    u8 *nextPacket;
    GpuCmdPacket *packet;
    DrawAreaRect *rectPtr;

    if (g_DrawBufferIndex != 0) {
        arg1 += 0xE0;
    }
    packet = NULL;
    rect.x = arg0;
    oldPacket = g_DrawPacketCursor;
    rectPtr = &rect;
    rect.y = arg1;
    rect.w = arg2;
    nextPacket = oldPacket + sizeof(GpuCmdPacket);
    rect.h = arg3;
    if (nextPacket < (g_DrawPacketArenaBase + 0x4000)) {
        g_DrawPacketCursor = nextPacket;
        packet = (GpuCmdPacket *) oldPacket;
    } else {
        BoundsCheck_AssertStub(1);
    }
    if (packet != NULL) {
        SetDrawArea(packet, rectPtr);
    }
    packet->u0.tag = (packet->u0.tag & 0xFF000000) | (*g_DrawOrderingTableEntry & 0xFFFFFF);
    *g_DrawOrderingTableEntry = (*g_DrawOrderingTableEntry & 0xFF000000) | ((s32) packet & 0xFFFFFF);
}

void Draw_AllocPrimRect(void) {
    DrawAreaRect rect;
    u8 *oldPacket;
    u8 *nextPacket;
    GpuCmdPacket *packet;
    DrawAreaRect *rectPtr;

    rect.w = 0x140;
    rect.x = 0;
    rect.y = 0;
    rect.h = 0xE0;
    if (g_DrawBufferIndex != 0) {
        rect.y = 0xE0;
    }
    packet = NULL;
    oldPacket = g_DrawPacketCursor;
    nextPacket = oldPacket + sizeof(GpuCmdPacket);
    rectPtr = &rect;
    if (nextPacket < (g_DrawPacketArenaBase + 0x4000)) {
        g_DrawPacketCursor = nextPacket;
        packet = (GpuCmdPacket *) oldPacket;
    } else {
        BoundsCheck_AssertStub(1);
    }
    if (packet != NULL) {
        SetDrawArea(packet, rectPtr);
    }
    packet->u0.tag = (packet->u0.tag & 0xFF000000) | (*g_DrawOrderingTableEntry & 0xFFFFFF);
    *g_DrawOrderingTableEntry = (*g_DrawOrderingTableEntry & 0xFF000000) | ((s32) packet & 0xFFFFFF);
}
