#include "pe1/battle_runtime.h"
#include "pe1/battle_status.h"
void Battle_BuildStatusPrimHeader(RenderObjectEntity *, s8);
void Battle_DrawSpellName(void);
void Battle_DrawStatusSymbol(s8);
void Battle_DrawStatusOverlay(RenderObjectEntity *object, int mode, int outOfRange, int entityId) {
    register int modeArg asm("$5") = mode;
    if ((s8)modeArg != 8) {
        register int modeMinusOne asm("$2") = modeArg - 1;
        if ((u8)modeMinusOne < 2) {
            u32 slot = D_8009CDDC;
            D_8009E358[slot * 3].x0 = 0x78;
            D_8009E358[slot * 3].y0 = 0x68;
        } else {
            Battle_BuildStatusPrimHeader(object, (s8)outOfRange);
        }
        switch ((s8)mode) {
        case 0:
            Battle_LayoutStatusPrimRow(0x28);
            Battle_DrawSpellName();
            Battle_DrawATBGauge();
            Battle_DrawStatusValue((s16)entityId, 0x1E);
            break;
        case 1:
        case 2:
            Battle_LayoutStatusPrimRow(0x20);
            Battle_DrawStatusSymbol((s8)mode);
            Battle_DrawATBGauge();
            break;
        case 3: {
            u32 slot;
            u32 packetOffset;
            register u8 *packetBase asm("$7");
            RenderSpritePacket *packet;
            u8 *packetTagBase;
            Battle_LayoutStatusPrimRow(0x1C);
            slot = D_8009CDDC;
            packetOffset = slot * 28;
            packetBase = (u8 *)&D_8009E730[0].sprite;
            packet = (RenderSpritePacket *)(packetBase + packetOffset);
            packet->x = D_8009E358[slot * 3].x0 + 0x1C;
            packet->y = D_8009E358[slot * 3].y0 + 8;
            /* Keep this assignment at the call to preserve the original schedule. */
            AddPrim((unsigned int *)D_800B0E38.ordering[slot] + 4,
                    (unsigned int *)((packetTagBase = packetBase - 8) + packetOffset));
            Battle_DrawStatusValue((s16)entityId, 0x12);
            break;
        }
        case 4:
            Battle_LayoutStatusPrimRow(0x28);
            Battle_DrawStatusSymbol((s8)mode);
            Battle_DrawATBGauge();
            Battle_DrawStatusValue((s16)entityId, 0x1E);
            break;
        case 5:
        case 6:
        case 7:
            Battle_LayoutStatusPrimRow(0x20);
            Battle_DrawStatusSymbol((s8)mode);
            Battle_DrawStatusValue((s16)entityId, 0x16);
            break;
        }
    } else {
        u32 slot = D_8009CDDC;
        D_8009E358[slot * 3].x0 = 0x78;
        D_8009E358[slot * 3].y0 = 0x6C;
        Battle_LayoutStatusPrimRow(0x18);
        Battle_DrawStatusSymbol(8);
    }
    if ((s32)((D_8009D278->action->turnWord >> 4) & 3) >= 2)
        Battle_DrawTargetHighlight();
}
