/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 Render_CheckParticleBounds(void *arg0) {
    s32 i;
    s32 ff;
    s32 offset;
    u8 *cursor;
    s32 matched;
    register s32 n asm("$3");
    s32 needed;
    register void *out asm("$6");
    register s32 limit asm("$2");

    i = 0;
    if (M2C_FIELD(arg0, u8 *, 0xE9) != 0) {
        ff = 0xFF;
        offset = 0;
        do {
            cursor = M2C_FIELD(arg0, u8 **, 0x20);
            matched = 0;
            n = 5;
            do {
                if (*cursor++ == i) {
                    matched += 1;
                }
                n -= 1;
            } while (n >= 0);
            needed = M2C_FIELD(offset + M2C_FIELD(arg0, s32 *, 4), u8 *, 2);
            cursor = M2C_FIELD(arg0, u8 **, 0x20);
            n = 0;
            if (needed == 0) {
                needed = 1;
            }
            out = arg0;
            do {
                if (*cursor++ == i) {
                    if (matched < needed) {
                        M2C_FIELD(out, u8 *, 0x5D) = ff;
                        matched -= 1;
                    } else {
                        M2C_FIELD(out, u8 *, 0x5D) = i;
                    }
                }
                n += 1;
                out += 1;
            } while (n < 6);
            limit = M2C_FIELD(arg0, u8 *, 0xE9);
            i += 1;
            offset += 5;
        } while (i < limit);
    }
    M2C_FIELD(arg0, u8 *, 0x46) = 0xFE;
    return 0;
}
