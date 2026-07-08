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

void Draw_AllocPrimRectFull(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    RECT rect;
    u32 oldPacket;
    u32 nextPacket;
    GpuCmdPacket *packet;
    RECT *rectPtr;

    if (g_DrawBufferIndex != 0) {
        arg1 += 0xE0;
    }
    packet = NULL;
    rect.x = arg0;
    oldPacket = g_ActiveDrawBuffer;
    rectPtr = &rect;
    rect.y = arg1;
    rect.w = arg2;
    nextPacket = oldPacket + 0xC;
    rect.h = arg3;
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
