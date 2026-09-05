#include "common.h"
void Render_LoadFontGlyph(u8 code);

extern u8 D_80091A1D;
extern u8 D_80091A1E;
extern u8 D_80091A1F;
extern u8 D_80091A1F_rd[] __asm__("D_80091A1F");
extern u8 *D_80091A28;
extern u8 D_8009EE22[];

u8 Render_StepFontLoad(void) {
    u8 state;
    u8 next;
    register u8 code asm("$4");
    u8 *hdr;
    u8 *p;
    u8 *q;
    u8 *t;
    s32 slot;
    register s32 digit asm("$3");
    s32 i;
    s32 count;
    register s32 found asm("$7");
    u8 *statep;
    int stack_pad[3];

    statep = &D_80091A1D;
    state = *statep;
    if (state < 3U) {
        *statep = 1;
        return 0xFFU;
    }

    next = state - 1;
    *statep = next;
    code = D_8009EE22[next];
    D_80091A1E = code;
    Render_LoadFontGlyph(code);

    digit = (u8)((u32)D_80091A1D % 10) != 0;
    found = 0;
    i = 0;
    asm("" : "=r"(digit), "=r"(i), "=r"(found)
        : "0"(digit), "1"(i), "2"(found));

    hdr = D_80091A28;
    count = hdr[3];
    p = hdr + 1;
    if (count > 0) {
        digit &= 0xFF;
        do {
            q = p + i;
            if (q[3] == digit) {
                found = i;
                i = p[2];
            }
            i++;
        } while (i < count);
        i = 0;
    }

    {
        s32 count2;
        register s32 wanted asm("$4");
        u8 *p2;
        slot = p[0x1B];
        p2 = p + 0x1B;
        if (slot > 0) {
            wanted = found & 0xFF;
            count2 = slot;
            do {
                q = p2 + i;
                if (q[1] == wanted) {
                    slot = i;
                    goto store;
                }
                i++;
            } while (i < count2);
        }
    }
    slot = 0xFF;

store:
        D_80091A1F = slot;
    __asm__ volatile("" : : : "memory");
    t = D_80091A28;
    return *(t + *(t + D_80091A1F_rd[0] + 0x1D) + 4);
}
