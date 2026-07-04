/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

typedef struct Rect {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} Rect;

typedef struct Prim {
    s32 tag;
    s32 code0;
    s32 code1;
} Prim;

M2C_UNK BoundsCheck_AssertStub();
void SetDrawArea(Prim *, Rect *);
extern u32 g_ActiveDrawBuffer;
extern s32 g_DrawPacketBufferBase;
extern s32 g_DrawBufferIndex;
extern s32 *g_OtListTail;

void Draw_AllocPrimRectFull(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    Rect rect;
    u32 oldPacket;
    u32 nextPacket;
    Prim *packet;
    Rect *rectPtr;

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
        packet = (Prim *) oldPacket;
    } else {
        BoundsCheck_AssertStub(1);
    }
    if (packet != NULL) {
        SetDrawArea(packet, rectPtr);
    }
    packet->tag = (packet->tag & 0xFF000000) | (*g_OtListTail & 0xFFFFFF);
    *g_OtListTail = (*g_OtListTail & 0xFF000000) | ((s32) packet & 0xFFFFFF);
}
