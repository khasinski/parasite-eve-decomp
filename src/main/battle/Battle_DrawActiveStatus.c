/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G8 --use-comm-section --expand-div */
/* MASPSX_FORCE_G0: 1 */
#include "pe1/battle_runtime.h"
#include "pe1/battle_status.h"
/* COMMON metadata selects GP-relative position loads in stock MASPSX.
 * Existing retail linker symbols supply storage. */
volatile u16 D_8009CE84, D_8009CE86;

/* Clamp active combatant gauges, draw ATB/PE bars and their background, then
 * draw HP and status icons. Matching debt: 12 pins and 6 empty barriers retain
 * the separate background branches. Two volatile position globals and five
 * volatile packet views preserve retail load/store order and repeated reads. */
void Battle_DrawActiveStatus(void)
{
    s16 maximumHP;
    register s16 backgroundHeight asm("$3");
    s32 maximumGauge;
    s32 spriteOffset;
    s32 gradientPairOffset;
    s32 currentGauge;
    s32 gradientOffset;
    s32 peWidth;
    s32 atbWidth;
    register s32 peProduct asm("$3");
    register u16 backgroundY;
    volatile RenderColorTilePacket *atbTrack;
    volatile RenderGouraudQuad *peFill;
    volatile RenderSpritePacket *atbLabel;
    RenderSpritePacket *peLabel;
    u8 *peRemainderBase;
    volatile RenderGouraudQuad *atbFill;
    RenderSpritePacket *atbMarker;
    volatile RenderGouraudQuad *peRemainder;
    register RenderColorTilePacket *background asm("$2");
    if (!(D_8009D1A0 & 2))
    {
        D_8009D278 = (Combatant *) D_8009D254->core;
    }
    if ((u16) D_8009D278->hpAlive >= 0x2329U)
    {
        D_8009D278->hpAlive = 0x2328;
    }
    currentGauge = D_8009D278->exp_or_acc;
    maximumGauge = D_8009D278->maxAtk;
    if (maximumGauge < currentGauge)
    {
        D_8009D278->exp_or_acc = maximumGauge;
    }
    else if (currentGauge < 0)
    {
        D_8009D278->exp_or_acc = 0;
    }
    maximumHP = (s16) D_8009D278->maxHP;
    if (maximumHP < (s16) D_8009D278->curHP)
    {
        D_8009D278->curHP = (u16) maximumHP;
    }
    atbWidth = (D_8009D278->hpAlive * 0x38) / 9000;
    peProduct = (s16)(D_8009D278->exp_or_acc >> 16) * 0x38;
    peWidth = peProduct / (s16)(D_8009D278->maxAtk >> 16);
    if (D_8009D1A0 & 2)
    {
        atbTrack = (volatile RenderColorTilePacket *)(void *)((D_8009CDDC * 0x10) + D_8009E098);
        atbTrack->x0 = (s16) (D_8009CE84 + 8);
        gradientOffset = D_8009CDDC * 0x24;
        atbTrack->y0 = (s16) (D_8009CE86 + 4);
        atbFill = (volatile RenderGouraudQuad *)(void *)(gradientOffset + D_800B00E8);
        atbFill->x0 = (s16) (D_8009CE84 + 8);
        atbFill->y0 = (s16) (D_8009CE86 + 4);
        atbFill->x1 = (s16) (D_8009CE84 + 8 + atbWidth);
        atbFill->y1 = (s16) (D_8009CE86 + 4);
        atbFill->x2 = (s16) (D_8009CE84 + 8);
        spriteOffset = D_8009CDDC * 0x1C;
        atbFill->y2 = (s16) (D_8009CE86 + 7);
        atbFill->x3 = (s16) (D_8009CE84 + 8 + atbWidth);
        atbFill->y3 = (s16) (D_8009CE86 + 7);
        atbMarker = (volatile RenderSpritePacket *)(void *)(spriteOffset + D_800B6928);
        atbMarker->x = (u16) *(u16 *)(D_800B00F8 + gradientOffset);
        atbMarker->y = (s16) (*(u16 *)(D_800B00FA + gradientOffset) - 2);
        atbLabel = (volatile RenderSpritePacket *)(void *)(spriteOffset + D_8009E0C0);
        atbLabel->x = (s16) (D_8009CE84 + 0x44);
        atbLabel->y = (s16) (D_8009CE86 + 3);
        AddPrim((u32 *) (D_800B0E38.ordering[D_8009CDDC] + 0x18), atbTrack);
        AddPrim((u32 *) (D_800B0E38.ordering[D_8009CDDC] + 0x14), (D_8009CDDC * 0x24) + D_800B00E8);
        AddPrim((u32 *) (D_800B0E38.ordering[D_8009CDDC] + 0x10), (D_8009CDDC * 0x1C) + (D_800B6928 - 8));
        AddPrim((u32 *) (D_800B0E38.ordering[D_8009CDDC] + 0x10), (D_8009CDDC * 0x1C) + (D_8009E0C0 - 8));
    }
    if (Menu_GetItemContextFlag() & 2)
    {
        gradientPairOffset = D_8009CDDC * 0x48;
        peFill = (volatile RenderGouraudQuad *)(void *)(gradientPairOffset + D_800B0130);
        peFill->x0 = (s16) (D_8009CE84 + 8);
        peFill->y0 = (s16) (D_8009CE86 + 0x19);
        peFill->x1 = (s16) (D_8009CE84 + 8 + peWidth);
        peFill->y1 = (s16) (D_8009CE86 + 0x19);
        peFill->x2 = (s16) (D_8009CE84 + 8);
        peFill->y2 = (s16) (D_8009CE86 + 0x1C);
        peFill->x3 = (s16) (D_8009CE84 + 8 + peWidth);
        peRemainderBase = D_800B0130 + 0x24;
        peFill->y3 = (s16) (D_8009CE86 + 0x1C);
        peRemainder = (volatile RenderGouraudQuad *)(void *)(gradientPairOffset + peRemainderBase);
        peRemainder->x0 = (u16) *(u16 *)(D_800B0140 + gradientPairOffset);
        peRemainder->y0 = (s16) (D_8009CE86 + 0x19);
        peRemainder->x1 = (s16) ((*(u16 *)(D_800B0140 + gradientPairOffset) + 0x38) - peWidth);
        peRemainder->y1 = (s16) (D_8009CE86 + 0x19);
        peRemainder->x2 = (u16) *(u16 *)(D_800B0140 + gradientPairOffset);
        peRemainder->y2 = (s16) (D_8009CE86 + 0x1C);
        peLabel = (volatile RenderSpritePacket *)(void *)((D_8009CDDC * 0x1C) + D_8009E328);
        peRemainder->x3 = (s16) ((*(u16 *)(D_800B0140 + gradientPairOffset) + 0x38) - peWidth);
        peRemainder->y3 = (s16) (D_8009CE86 + 0x1C);
        peLabel->x = (s16) (D_8009CE84 + 0x44);
        peLabel->y = (s16) (D_8009CE86 + 0x18);
        AddPrim((u32 *) (D_800B0E38.ordering[D_8009CDDC] + 0x14), peFill);
        AddPrim((u32 *) (D_800B0E38.ordering[D_8009CDDC] + 0x14), (D_8009CDDC * 0x48) + peRemainderBase);
        AddPrim((u32 *) (D_800B0E38.ordering[D_8009CDDC] + 0x10), (D_8009CDDC * 0x1C) + (D_8009E328 - 8));
        if (D_8009D1A0 & 2)
        {
            register s32 index asm("$3") = D_8009CDDC;
            register u8 *base asm("$4") = D_8009E070;
            background = (RenderColorTilePacket *)(base + index * 0x18);
            backgroundHeight = D_8009CE84;
            background->x0 = backgroundHeight;
            asm("" : : "r"(background) : "memory");
            backgroundY = D_8009CE86;
            backgroundHeight = 0x54;
            background->w = backgroundHeight;
            backgroundHeight = 0x20;
            background->h = backgroundHeight;
        }
        else
        {
            register s32 index asm("$3") = D_8009CDDC;
            register u8 *base asm("$4") = D_8009E070;
            background = (RenderColorTilePacket *)(base + index * 0x18);
            backgroundHeight = D_8009CE84;
            background->x0 = backgroundHeight;
            asm("" : : "r"(background) : "memory");
            backgroundY = D_8009CE86;
            backgroundHeight = 0x54;
            background->w = backgroundHeight;
            asm("" : : "r"(background) : "memory");
            backgroundHeight = 0x19;
            goto shiftBackgroundDown;
        }
    }
    else if (D_8009D1A0 & 2)
    {
        register s32 index asm("$3") = D_8009CDDC;
        register u8 *base asm("$4") = D_8009E070;
        background = (RenderColorTilePacket *)(base + index * 0x18);
        backgroundHeight = D_8009CE84;
        background->x0 = backgroundHeight;
        asm("" : : "r"(background) : "memory");
        backgroundY = D_8009CE86;
        backgroundHeight = 0x54;
        background->w = backgroundHeight;
        backgroundHeight = 0x19;
        background->h = backgroundHeight;
    }
    else
    {
        register s32 index asm("$3") = D_8009CDDC;
        register u8 *base asm("$4") = D_8009E070;
        background = (RenderColorTilePacket *)(base + index * 0x18);
        backgroundHeight = D_8009CE84;
        background->x0 = backgroundHeight;
        asm("" : : "r"(background) : "memory");
        backgroundY = D_8009CE86;
        backgroundHeight = 0x54;
        background->w = backgroundHeight;
        asm("" : : "r"(background) : "memory");
        backgroundHeight = 0x12;
    shiftBackgroundDown:
        background->h = backgroundHeight;
        backgroundY += 7;
    }
    background->y0 = backgroundY;
    {
        register s32 lastIndex asm("$2") = D_8009CDDC;
        AddPrim((u32 *)(D_800B0E38.ordering[lastIndex] + 0x1C), lastIndex * 0x18 + D_8009E068);
    }
    Battle_DrawEnemyHP((s16) D_8009D278->maxHP, (s16) D_8009D278->curHP);
    Gpu_DrawStatusIcons();
}
