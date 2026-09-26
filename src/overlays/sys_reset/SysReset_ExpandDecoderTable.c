#include "common.h"
extern const u8 D_8010CBFC[];
void SysReset_ExpandDecoderTable(u16 *table) {
    s32 distance = 0;
    const u8 *source = D_8010CBFC;
    /* Keep the output cursor in a2; the empty barrier preserves the token load order. */
    register u8 *output asm("$6") = (u8 *)table;
    u32 count;
    u32 token;
    s32 wordCount;
    do {
        token = *source;
        __asm__("" : "=r"(token) : "0"(token), "r"(source));
        count = token & 255;
        ++source;
        if (count < 240) {
            if (distance != 0) {
                for (; (s32)count >= 0; --count) {
                    *output = output[-distance];
                    ++output;
                }
            } else {
                for (; (s32)count >= 0; --count)
                    *output++ = *source++;
            }
        } else {
            distance = 0;
            if (count != 240) {
                u32 nextByte;
                nextByte = *source++;
                distance = ((count << 8) | nextByte) - 0xF0FF;
            }
        }
    } while (distance != 3840);
    for (wordCount = 4; wordCount <= 0x87FF; ++wordCount)
        table[wordCount] ^= table[wordCount - 4];
}
