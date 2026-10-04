/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/battle_status.h"

/* Draw a floating damage/recovery number, or the enemy MISS label.
 * Each frame has ten panel slots (0x578 / 0x8C), each with five 28-byte
 * texture-page/sprite packets. Repeated byte stores deliberately retain
 * the original index reloads and separate symbol views.
 * Matching debt: 14 pins, four empty barriers, and 48 unused stack bytes.
 * The stack reservation preserves the retail frame; its original purpose
 * is unknown. GCC rotates the digit-conversion condition into two blocks.
 */
void Battle_DrawStatusPanel(s32 mode, BattleStatusPanel *input) {
    BattleStatusPanel *panel = input;
    u8 digits[8];
    volatile u8 matchingStackReserve[48];
    s32 screenY;
    s16 panelValue;
    register s32 number asm("$6");
    s32 screenX;
    s32 panelOffset;
    s32 digitIndex;
    s32 frameOffset;
    register s32 missFrameOffset asm("$6");
    s32 digitOffset;
    s32 digitPanelOffset;
    s32 signedDigitIndex;
    register s32 colorOffset asm("$5");
    s8 nextDigit;
    s8 lastDigit;
    register s32 timer asm("$5");
    register RenderSpritePacket *missSprite asm("$2");
    RenderSpritePacket *digitSprite;

    panelValue = panel->value;
    number = panelValue;
    if ((panelValue < 0) && (number = 0, ((mode & 0xFF) == 1))) {
        register s32 frame asm("$2") = g_BattlePanelInitialDrawSlotView[0];
        register s32 slotIndex asm("$4") = g_BattlePanelPacketIndex;
        register s32 x asm("$6") = panel->x;
        register s32 y asm("$7");
        asm("" : : "r"(x) : "memory");
        timer = panel->timer;
        asm("" : : "r"(x), "r"(timer) : "memory");
        y = panel->y;
        *(D_800B01CC + ((slotIndex * 0x8C) + (frame * 0x578))) = timer * 4;
        asm("" : : : "memory");
        screenX = x - 16;
        asm("" : : "r"(screenX));
        timer -= 30;
        screenY = y + timer;
        *(D_800B01CD + ((g_BattlePanelPacketIndex * 0x8C) + (g_BattlePanelDrawSlotView[0] * 0x578))) = 0;
        *(D_800B01CE + ((g_BattlePanelPacketIndex * 0x8C) + (g_BattlePanelDrawSlotView[0] * 0x578))) = 0;
        *(D_800B01D4 + ((g_BattlePanelPacketIndex * 0x8C) + (g_BattlePanelDrawSlotView[0] * 0x578))) = 0x50;
        *(D_800B01D5 + ((g_BattlePanelPacketIndex * 0x8C) + (g_BattlePanelDrawSlotView[0] * 0x578))) = 0xE0;
        {
            register s32 slot asm("$7") = g_BattlePanelDrawSlotView[0];
            missFrameOffset = slot * 0x578;
            panelOffset = g_BattlePanelPacketIndex * 0x8C;
            missSprite = (RenderSpritePacket *)(missFrameOffset + panelOffset + ((u8 *)D_800B01C8));
            missSprite->width = 0x18;
            missSprite->x = screenX;
            missSprite->y = screenY;
            missSprite->height = 8;
            AddPrim(((u32 *)D_800B0E38.ordering[slot]) + 7, panelOffset + ((((u8 *)D_800B01C8) - 8) + missFrameOffset));
        }
    } else {
        s16 value = number;
        s16 quotient;
        s8 index = 0;
        lastDigit = 0;
        while ((quotient = (s16)value / 10,
        digits[index] = value - (s16)quotient * 10,
        value = quotient, (s16)value != 0)) {
            index = lastDigit + 1;
            lastDigit = index;
        }
        {
            s32 count = lastDigit;
            register s32 width asm("$2") = (count + 1) * 4;
            register s32 coordinate asm("$4") = panel->x;
            screenX = coordinate - width;
            width = panel->timer;
            coordinate = panel->y;
            width -= 30;

            screenY = coordinate + width;
        }

        if (lastDigit >= 0) {
            do {
                switch (panel->style) {
                    case 0:
                    colorOffset = lastDigit * 0x1C;
                    ((RenderSpritePacket *)(colorOffset + ((g_BattlePanelPacketIndex * 0x8C) + (g_BattlePanelDrawSlotView[0] * 0x578)) + ((u8 *)D_800B01C8)))->color.bytes.r = panel->timer * 4;
                    ((RenderSpritePacket *)(colorOffset + ((g_BattlePanelPacketIndex * 0x8C) + (g_BattlePanelDrawSlotView[0] * 0x578)) + ((u8 *)D_800B01C8)))->color.bytes.g = panel->timer * 4;
                    ((RenderSpritePacket *)(colorOffset + ((g_BattlePanelPacketIndex * 0x8C) + (g_BattlePanelDrawSlotView[0] * 0x578)) + ((u8 *)D_800B01C8)))->color.bytes.b = panel->timer * 4;
                    break;
                    case 1:
                    colorOffset = lastDigit * 0x1C;
                    {
                        register RenderSpritePacket *sprite asm("$3") = ((RenderSpritePacket *)(colorOffset + ((g_BattlePanelDrawSlotView[0] * 0x578) + (g_BattlePanelPacketIndex * 0x8C)) + ((u8 *)D_800B01C8)));
                        sprite->color.bytes.r = 0;
                    }
                    ((RenderSpritePacket *)(colorOffset + ((g_BattlePanelPacketIndex * 0x8C) + (g_BattlePanelDrawSlotView[0] * 0x578)) + ((u8 *)D_800B01C8)))->color.bytes.g = panel->timer * 4;
                    ((RenderSpritePacket *)(colorOffset + ((g_BattlePanelPacketIndex * 0x8C) + (g_BattlePanelDrawSlotView[0] * 0x578)) + ((u8 *)D_800B01C8)))->color.bytes.b = 0;
                    break;
                    case 2:
                    colorOffset = lastDigit * 0x1C;
                    ((RenderSpritePacket *)(colorOffset + ((g_BattlePanelPacketIndex * 0x8C) + (g_BattlePanelDrawSlotView[0] * 0x578)) + ((u8 *)D_800B01C8)))->color.bytes.r = panel->timer * 4;
                    ((RenderSpritePacket *)(colorOffset + ((g_BattlePanelPacketIndex * 0x8C) + (g_BattlePanelDrawSlotView[0] * 0x578)) + ((u8 *)D_800B01C8)))->color.bytes.g = panel->timer * 4;
                    ((RenderSpritePacket *)(colorOffset + ((g_BattlePanelPacketIndex * 0x8C) + (g_BattlePanelDrawSlotView[0] * 0x578)) + ((u8 *)D_800B01C8)))->color.bytes.b = 0;
                    break;
                    case 3:
                    colorOffset = lastDigit * 0x1C;
                    ((RenderSpritePacket *)(colorOffset + ((g_BattlePanelPacketIndex * 0x8C) + (g_BattlePanelDrawSlotView[0] * 0x578)) + ((u8 *)D_800B01C8)))->color.bytes.r = panel->timer * 4;
                    {
                        register RenderSpritePacket *sprite asm("$3") = ((RenderSpritePacket *)(colorOffset + ((g_BattlePanelDrawSlotView[0] * 0x578) + (g_BattlePanelPacketIndex * 0x8C)) + ((u8 *)D_800B01C8)));
                        sprite->color.bytes.g = 0;
                    }
                    ((RenderSpritePacket *)(colorOffset + ((g_BattlePanelPacketIndex * 0x8C) + (g_BattlePanelDrawSlotView[0] * 0x578)) + ((u8 *)D_800B01C8)))->color.bytes.b = panel->timer * 4;
                    break;
                }
                signedDigitIndex = lastDigit << 24;
                digitIndex = signedDigitIndex >> 0x18;
                digitOffset = digitIndex * 0x1C;
                screenX += 8;
                ((RenderSpritePacket *)(digitOffset + ((g_BattlePanelPacketIndex * 0x8C) + (g_BattlePanelDrawSlotView[0] * 0x578)) + ((u8 *)D_800B01C8)))->u = (s8) (digits[digitIndex] * 8);
                ((RenderSpritePacket *)(digitOffset + ((g_BattlePanelPacketIndex * 0x8C) + (g_BattlePanelDrawSlotView[0] * 0x578)) + ((u8 *)D_800B01C8)))->v = 0xE0;
                {
                    s32 slot = g_BattlePanelDrawSlotView[0];
                    frameOffset = slot * 0x578;
                    digitPanelOffset = g_BattlePanelPacketIndex * 0x8C;
                    digitSprite = (RenderSpritePacket *)(digitOffset + (frameOffset + digitPanelOffset) + ((u8 *)D_800B01C8));
                    digitSprite->x = screenX;
                    digitSprite->y = screenY;
                    digitSprite->width = 8;
                    digitSprite->height = 8;
                    AddPrim(((u32 *)D_800B0E38.ordering[slot]) + 7, ((digitPanelOffset + ((((u8 *)D_800B01C8) - 8) + frameOffset)) + digitOffset));
                }
                nextDigit = lastDigit - 1;
                lastDigit = nextDigit;
            } while (nextDigit >= 0);
        }
    }
    g_BattlePanelPacketIndex += 1;
}
