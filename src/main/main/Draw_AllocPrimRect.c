/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

#include "pe1/psyq_gpu.h"

M2C_UNK BoundsCheck_AssertStub();
void SetDrawArea(GpuCmdPacket *, RECT *);
extern u32 g_ActiveDrawBuffer;
extern s32 g_DrawPacketBufferBase;
extern s32 g_DrawBufferIndex;
extern s32 *g_OtListTail;

void Draw_AllocPrimRect(void) {
    RECT rect;
    u32 oldPacket;
    u32 nextPacket;
    GpuCmdPacket *packet;
    RECT *rectPtr;

    rect.w = 0x140;
    rect.x = 0;
    rect.y = 0;
    rect.h = 0xE0;
    if (g_DrawBufferIndex != 0) {
        rect.y = 0xE0;
    }
    packet = NULL;
    oldPacket = g_ActiveDrawBuffer;
    nextPacket = oldPacket + 0xC;
    rectPtr = &rect;
    if (nextPacket < (u32) (g_DrawPacketBufferBase + 0x4000)) {
        g_ActiveDrawBuffer = nextPacket;
        packet = (GpuCmdPacket *) oldPacket;
    } else {
        BoundsCheck_AssertStub(1);
    }
    if (packet != NULL) {
        SetDrawArea(packet, rectPtr);
    }
    packet->u0.tag = (packet->u0.tag & 0xFF000000) | (*g_OtListTail & 0xFFFFFF);
    *g_OtListTail = (*g_OtListTail & 0xFF000000) | ((s32) packet & 0xFFFFFF);
}
