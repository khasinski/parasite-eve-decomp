#include "common.h"

#include "vlc_build_externs.h"

void MdecDiag_ExpandDecoderTable(u16 *table) {
    s32 distance = 0;
    const u8 *source = g_MdecVlcTableSource;
    /* The empty barrier preserves the token load order. */
    u8 *output = (u8 *)table;
    u32 count;
    u8 token;
    s32 wordCount;
    do {
        token = *(const volatile u8 *)source;
        count = token;
        ++source;
        if (count < 240) {
            if (distance != 0) {
                for (; (int)count >= 0; --count) {
                    *output = output[-distance];
                    ++output;
                }
            } else {
                for (; (int)count >= 0; --count)
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
