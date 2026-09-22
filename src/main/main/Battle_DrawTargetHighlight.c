#include "pe1/battle_runtime.h"
#include "pe1/battle_status.h"

static inline RenderSpritePacket *HighlightSpriteAt(RenderSpritePacket *base, int slot, unsigned byte_offset)
{
    return (RenderSpritePacket *)(slot * 3 * sizeof(BattleGaugePrim) + byte_offset + (u32)base);
}

void Battle_DrawTargetHighlight(void)
{
    unsigned char index = 0;
    RenderSpritePacket *base;
    RenderSpritePacket *sprite;
    BattleGaugePrim *packets;
    int slot;
    unsigned slot_offset;
    register unsigned first_offset asm("$4");
    register unsigned index_offset asm("$5");
    register unsigned char texture_v asm("$18");
    if (((D_8009D278->action->turnWord >> 4) & 3) == 0) {
        return;
    }
    texture_v = 224;
    base = &D_8009E3B8[0][0].sprite;
    packets = (BattleGaugePrim *)((char *)base - PE1_OFFSETOF(BattleGaugePrim, sprite));
    do {
        if (index == g_BattleActiveTurnSlot - 1) {
            first_offset = index * sizeof(BattleGaugePrim);
            HighlightSpriteAt(base, g_ActiveDrawSlot, first_offset)->u = index * 24 + 104;
        } else {
            first_offset = index * sizeof(BattleGaugePrim);
            HighlightSpriteAt(base, g_ActiveDrawSlot, first_offset)->u = 176;
        }
        HighlightSpriteAt(base, g_ActiveDrawSlot, first_offset)->v = texture_v;
        /* Preserve the retail store before recomputing the next packet offset. */
        __asm__ volatile("" : : : "memory");
        index_offset = index * sizeof(BattleGaugePrim);
        slot = g_ActiveDrawSlot;
        slot_offset = slot * 3 * sizeof(BattleGaugePrim);
        sprite = (RenderSpritePacket *)(slot_offset + index_offset + (u32)base);
        sprite->x = D_8009E358[slot * 3].x0 + index * 24;
        sprite->y = D_8009E358[slot * 3].y0 - 8;
        AddPrim((unsigned *)D_800B0E38.ordering[slot] + 7,
                (unsigned *)(slot_offset + (index_offset + (u32)packets)));
        ++index;
    } while (index < (int)((D_8009D278->action->turnWord >> 4) & 3));
}
