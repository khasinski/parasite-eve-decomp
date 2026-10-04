/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G8 --use-comm-section --expand-div */
/* MASPSX_FORCE_G0: 1 */
#include "pe1/battle_runtime.h"
#include "pe1/battle_status.h"

/* COMMON metadata selects GP-relative loads;
 * retail symbols supply storage. */
u16 D_8009CE84;
volatile u16 D_8009CE86;
extern s32 D_8009D1E8;

/* Draw current/maximum HP from reversed decimal digits and the HP label.
 * Matching debt: 14 register pins and 9 empty barriers preserve retail scheduling;
 * the volatile Y origin and label view preserve the final coordinate order. */
void Battle_DrawEnemyHP(s16 maximum, s16 current)
{
    u8 digits[8];
    register u8 *sprites asm("$18");
    register s16 savedCurrent asm("$19") = current;
    s16 screenY;
    s16 screenX;
    register s32 digitIndex asm("$4");
    s32 pulseByte;
    s32 labelOffset;
    s32 maximumDigitOffset;
    register s32 currentDigitOffset asm("$6");
    register s32 shiftedIndex asm("$3");
    register s8 slashIndex asm("$4");
    s8 maximumLastIndex;
    s8 nextMaximumIndex;
    s8 missingDigits;
    s8 nextCurrentIndex;
    register s8 maximumLastDigit asm("$16");
    s8 maximumIndex;
    s8 currentIndex;
    register u32 maximumPhase asm("$3");
    register u32 currentPhase asm("$3");
    u32 maximumPulse;
    u32 currentPulse;
    volatile RenderSpritePacket *label;
    RenderSpritePacket *maximumSprite;
    RenderSpritePacket *currentSprite;

    /* Extract maximum HP digits least-significant first, then append slash. */
    asm("" : : "r"(maximum), "r"(savedCurrent));
    maximumLastDigit = 0;
    screenY = D_8009CE86 + 11;
    {
        s16 value = maximum;
        s16 quotient;
        s8 index = 0;
        while ((quotient = value / 10, digits[index] = value - (s16)quotient * 10,
               value = quotient, (s16)value != 0)) {
            index = maximumLastDigit + 1;
            maximumLastDigit = index;
        }
    }
    slashIndex = maximumLastDigit + 1;
    maximumIndex = slashIndex;
    asm volatile("" : "=r"(slashIndex) : "0"(slashIndex));
    digits[slashIndex] = 0xA;
    maximumLastIndex = maximumIndex;
    screenX = (D_8009CE84 - (slashIndex * 6)) + 56;
    if (slashIndex >= 0) {
        u32 gray = 128;
        sprites = D_8009E1D8;
        asm volatile("" : "=r"(maximumIndex) : "0"(maximumIndex), "r"(gray));
        shiftedIndex = (u32)maximumIndex << 24;
        do {
            s32 frame = D_8009CDDC;
            s32 frameOffset;
            digitIndex = shiftedIndex >> 24;
            frameOffset = frame * 140;
            {
                s32 column = digitIndex * 7;
                maximumDigitOffset = column * 4;
            }
            ((RenderSpritePacket *)(frameOffset + maximumDigitOffset + (u32)sprites))->u = (s8) (digits[digitIndex] * 8);
            ((RenderSpritePacket *)((D_8009CDDC * 140) + maximumDigitOffset + (u32)sprites))->v = 0xE8;
            maximumSprite = (RenderSpritePacket *)((D_8009CDDC * 140) + maximumDigitOffset + (u32)sprites);
            maximumSprite->x = screenX;
            maximumSprite->y = screenY;
            if (!(((Combatant *)D_8009D254->core)->stateFlags & 0x800) || (digitIndex == maximumLastIndex)) {
                maximumSprite->color.bytes.r = gray;
                ((RenderSpritePacket *)((D_8009CDDC * 140) + maximumDigitOffset + (u32)sprites))->color.bytes.g = gray;
            }
            else {
                {
                    register u32 clock asm("$2") = D_8009D1E8;
                    maximumPhase = clock & 31;
                }
                maximumPulse = maximumPhase;
                asm volatile("" : "=r"(maximumPulse) : "0"(maximumPulse));
                if (maximumPulse >= 17U) {
                    maximumPulse = 32 - maximumPhase;
                }
                maximumSprite->color.bytes.r = (s8) (((maximumPulse & 255) * 6) + 32);
                ((RenderSpritePacket *)((D_8009CDDC * 140) + maximumDigitOffset + (u32)sprites))->color.bytes.g = (s8) (maximumPulse + 96);
            }
            ((RenderSpritePacket *)((D_8009CDDC * 140) + maximumDigitOffset + (u32)sprites))->color.bytes.b = gray;
            screenX += 6;
            {
                s32 frameEnd = D_8009CDDC;
                u32 otOffset = frameEnd * 4;
                s32 frameOffset = frameEnd * 140;
                {
                    s32 index = maximumIndex;
                    u8 *page = index * 28 + D_8009E1D0;
                    AddPrim((u32 *)(*(u8 **)((u8 *)D_800B0E38.ordering + otOffset) + 20), (u32 *)(frameOffset + (u32)page));
                }
            }
            nextMaximumIndex = maximumIndex - 1;
            maximumIndex = nextMaximumIndex;
            asm volatile("" : "=r"(maximumIndex) : "0"(maximumIndex));
            shiftedIndex = (u32)maximumIndex << 24;
        } while (nextMaximumIndex >= 0);
    }

    /* Align current HP against the maximum field and draw it from left to right. */
    currentIndex = 0;
    {
        s16 x = D_8009CE84 - ((((s32) ((u32)maximumLastIndex << 24) >> 23) + 1) * 6);
        screenX = x + 62;
    }
    {
        s16 value = savedCurrent;
        s16 quotient;
        s8 index = 0;
        while ((quotient = value / 10, digits[index] = value - (s16)quotient * 10,
               value = quotient, (s16)value != 0)) {
            index = currentIndex + 1;
            currentIndex = index;
        }
    }
    missingDigits = (maximumLastIndex - currentIndex) - 1;
    if (missingDigits > 0) {
        screenX += missingDigits * 6;
    }
    if (currentIndex >= 0) {
        sprites = D_8009E0F8;
        do {
            register s32 frame asm("$2") = D_8009CDDC;
            s32 frameOffset;
            digitIndex = currentIndex;
            frameOffset = frame * 112;
            asm volatile("" : "=r"(frameOffset) : "0"(frameOffset));
            {
                register s32 column asm("$2") = digitIndex * 7;
                asm("" : "=r"(column) : "0"(column));
                currentDigitOffset = column * 4;
            }
            ((RenderSpritePacket *)(frameOffset + currentDigitOffset + (u32)sprites))->u = (s8) (digits[digitIndex] * 8);
            asm volatile("" : : "r"(digitIndex));
            ((RenderSpritePacket *)((D_8009CDDC * 112) + currentDigitOffset + (u32)sprites))->v = 0xE8;
            currentSprite = (RenderSpritePacket *)((D_8009CDDC * 112) + currentDigitOffset + (u32)sprites);
            currentSprite->x = screenX;
            currentSprite->y = screenY;
            if (((Combatant *)D_8009D254->core)->stateFlags & 0x400) {
                {
                    register u32 clock asm("$2") = D_8009D1E8;
                    currentPhase = clock & 31;
                }
                currentPulse = currentPhase;
                asm volatile("" : "=r"(currentPulse) : "0"(currentPulse));
                if (currentPulse >= 17U) {
                    currentPulse = 32 - currentPhase;
                }
                pulseByte = currentPulse & 255;
                currentSprite->color.bytes.r = (s8) (pulseByte * 8);
                ((RenderSpritePacket *)((D_8009CDDC * 112) + currentDigitOffset + (u32)sprites))->color.bytes.g = 128;
                ((RenderSpritePacket *)((D_8009CDDC * 112) + currentDigitOffset + (u32)sprites))->color.bytes.b = (s8) ((pulseByte * 6) + 32);
            }
            else {
                currentSprite->color.bytes.r = 128;
                ((RenderSpritePacket *)((D_8009CDDC * 112) + currentDigitOffset + (u32)sprites))->color.bytes.g = 128;
                ((RenderSpritePacket *)((D_8009CDDC * 112) + currentDigitOffset + (u32)sprites))->color.bytes.b = 128;
            }
            screenX += 6;
            {
                s32 frameEnd = D_8009CDDC;
                u32 otOffset = frameEnd * 4;
                s32 frameOffset = frameEnd * 112;
                {
                    s32 index = currentIndex;
                    u8 *page = index * 28 + D_8009E0F0;
                    AddPrim((u32 *)(*(u8 **)((u8 *)D_800B0E38.ordering + otOffset) + 20), (u32 *)(frameOffset + (u32)page));
                }
            }
            nextCurrentIndex = currentIndex - 1;
            currentIndex = nextCurrentIndex;
        } while (nextCurrentIndex >= 0);
    }

    /* Place the HP label after the number pair. */
    {
        register s32 frame asm("$3") = D_8009CDDC;
        labelOffset = frame * 28;
        label = (RenderSpritePacket *)(labelOffset + D_8009E2F0);
        label->x = (s16) (D_8009CE84 + 0x44);
        label->y = (s16) (D_8009CE86 + 0xE);
        AddPrim((u32 *) (D_800B0E38.ordering[frame] + 0x10), (u32 *)(labelOffset + (D_8009E2F0 - 8)));
    }
}
