/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;

#define NULL ((void *)0)

#include "../../../tools/m2c/m2c_macros.h"

void *Draw_LookupGlyphDescriptor(s32 index) __asm__("func_8005DADC");
void BoundsCheck_AssertStub(s32 arg0);

extern u32 g_ActiveDrawBuffer;
extern s32 g_DrawPacketBufferBase;
extern s32 g_DrawTextDimmed;
extern s32 g_DrawPrimColor;
extern s32 g_DrawColorShaded;
extern s32 *g_OtListTail;
extern u16 g_TextCursorX;
extern u16 g_TextCursorY;

void Draw_EmitGlyph(s32 arg0, s32 arg1) {
    u8 *glyph;
    u32 oldPacket;
    u32 nextPacket;
    u32 packet;
    u16 base_x;
    u16 base_y;
    s32 temp;
    u32 oldTag;
    s32 *ot;

    packet = 0;
    glyph = Draw_LookupGlyphDescriptor(arg0);
    oldPacket = g_ActiveDrawBuffer;
    nextPacket = oldPacket + 0x28;
    if (nextPacket < (u32)(g_DrawPacketBufferBase + 0x4000)) {
        g_ActiveDrawBuffer = nextPacket;
        packet = oldPacket;
    } else {
        BoundsCheck_AssertStub(1);
    }

    if (packet != 0) {
        if (g_DrawTextDimmed != 0) {
            M2C_FIELD(packet, s32 *, 4) = g_DrawColorShaded;
        } else {
            M2C_FIELD(packet, s32 *, 4) = g_DrawPrimColor;
        }
        M2C_FIELD(packet, s8 *, 3) = 9;
        M2C_FIELD(packet, s8 *, 7) = 0x2C;
    }

    {
        register u32 ptr asm("$4");

        base_x = g_TextCursorX;
        base_y = g_TextCursorY;
        ptr = packet;
        M2C_FIELD(ptr, volatile u16 *, 0x18) = base_x;
        M2C_FIELD(ptr, volatile u16 *, 8) = base_x;
        M2C_FIELD(ptr, volatile u16 *, 0x12) = base_y;
        M2C_FIELD(ptr, volatile u16 *, 0xA) = base_y;

        {
            s32 sum;
            s32 glyphDim;

            asm volatile(
                "lbu %1, 0x4(%2)\n"
                "lhu %0, 0x8(%3)\n"
                "nop\n"
                "addu %0, %0, %1"
                : "=&r"(sum), "=&r"(glyphDim)
                : "r"(glyph), "r"(ptr)
                : "memory");
            temp = sum;
        }
        M2C_FIELD(ptr, s16 *, 0x20) = temp;
        M2C_FIELD(ptr, s16 *, 0x10) = temp;

        {
            s32 sum;
            s32 glyphDim;

            asm volatile(
                "lbu %1, 0x5(%2)\n"
                "lhu %0, 0xA(%3)\n"
                "nop\n"
                "addu %0, %0, %1"
                : "=&r"(sum), "=&r"(glyphDim)
                : "r"(glyph), "r"(ptr)
                : "memory");
            temp = sum;
        }
        M2C_FIELD(ptr, s16 *, 0x22) = temp;
        M2C_FIELD(ptr, s16 *, 0x1A) = temp;

        if (arg1 == 2) {
            temp = glyph[0] + glyph[4] - 1;
            M2C_FIELD(ptr, u8 *, 0x1C) = temp;
            M2C_FIELD(ptr, u8 *, 0xC) = temp;
            temp = glyph[1] + glyph[5] - 1;
            M2C_FIELD(ptr, u8 *, 0x15) = temp;
            M2C_FIELD(ptr, u8 *, 0xD) = temp;
            temp = glyph[0] - 1;
            M2C_FIELD(ptr, u8 *, 0x24) = temp;
            M2C_FIELD(ptr, u8 *, 0x14) = temp;
            temp = glyph[1] - 1;
            M2C_FIELD(ptr, u8 *, 0x25) = temp;
            M2C_FIELD(ptr, u8 *, 0x1D) = temp;
        }
    }

    oldTag = M2C_FIELD(packet, volatile u32 *, 0);
    {
        u32 mask24;
        u32 maskTop;
        register s32 tpage asm("$4");

        mask24 = 0xFFFFFF;
        tpage = M2C_FIELD(glyph, volatile u16 *, 2);
        M2C_FIELD(packet, u16 *, 0xE) = tpage;
        ot = g_OtListTail;
        maskTop = 0xFF000000;
        M2C_FIELD(packet, s16 *, 0x16) = 7;
        M2C_FIELD(packet, u32 *, 0) = (oldTag & maskTop) | (*ot & mask24);
        *ot = (*ot & maskTop) | (packet & mask24);
    }
}
