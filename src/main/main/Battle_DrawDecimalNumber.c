#include "pe1/battle_status.h"

int Battle_DrawDecimalNumber(void *buffer, short x, short y, short value, short mode)
{
    unsigned char digits[6];
    signed char index = 0;
    signed char last;
    short remaining;
    short quotient;
    BattleGaugePrim *packet;
    /* Collect least-significant digits, then draw from the highest place. */
    remaining = value;
    do {
        quotient = remaining / 10;
        digits[index] = remaining - quotient * 10;
        remaining = quotient;
        if (!remaining) {
            break;
        }
        ++index;
    } while (1);
    last = index;
    x -= last * 6;
    for (; index >= 0; --index) {
        unsigned digit;
        /* Retail computes the packet address as a 32-bit integer sum. */
        packet = (BattleGaugePrim *)(index * sizeof(*packet) + (u32)buffer);
        digit = digits[index];
        packet->sprite.v = 0xf2;
        packet->sprite.x = x;
        packet->sprite.y = y;
        packet->sprite.u = digit * 8;
        if (mode == 1) {
            packet->sprite.color.bytes.r = D_8009E460[g_ActiveDrawSlot].sprite.color.bytes.r + 40;
            packet->sprite.color.bytes.g = D_8009E460[g_ActiveDrawSlot].sprite.color.bytes.g;
            packet->sprite.color.bytes.b = D_8009E460[g_ActiveDrawSlot].sprite.color.bytes.b;
        }
        x += 6;
        AddPrim((unsigned *)D_800B0E38.ordering[g_ActiveDrawSlot] + 4, (unsigned *)packet);
    }
    return last;
}
